#include "jit/template_jit.h"

#include "jit/code_cache.h"
#include "jit/native_codegen.h"
#include "jit/runtime.h"
#include "jit/runtime_helpers.h"
#include "vm.h"

#include <cstring>
#include <vector>

namespace tinylang::jit {

static bool compile_supported(const Chunk& chunk, int self_fn_index) {
  if (chunk.arity != 1) return false;
  for (const auto& ins : chunk.code) {
    if (ins.op == Op::OP_CALL && ins.operand == self_fn_index) return false;
    switch (ins.op) {
      case Op::OP_PUSH_CONST:
      case Op::OP_PUSH_LOCAL:
      case Op::OP_POP:
      case Op::OP_ADD:
      case Op::OP_SUB:
      case Op::OP_MUL:
      case Op::OP_CALL:
      case Op::OP_RETURN:
      case Op::OP_NEG:
      case Op::OP_LT:
      case Op::OP_JMP:
      case Op::OP_JMP_IF_FALSE:
        break;
      default:
        return false;
    }
  }
  return true;
}

void* compile_function(VM& vm, FunctionObj& fn) {
#if !defined(__aarch64__)
  (void)vm;
  (void)fn;
  return nullptr;
#else
  if (!compile_supported(fn.chunk, fn.id)) return nullptr;

  NativeEmitter emitter;
  emitter.emit_prologue_arity1();

  const Chunk& chunk = fn.chunk;
  std::vector<std::size_t> bc_off(chunk.code.size());

  struct Fixup {
    std::size_t insn_index;
    std::size_t branch_offset;
    bool is_eq_branch;
  };
  std::vector<Fixup> fixups;

  int depth = 0;

  for (std::size_t i = 0; i < chunk.code.size(); ++i) {
    bc_off[i] = emitter.size();
    const Instruction& ins = chunk.code[i];
    switch (ins.op) {
      case Op::OP_PUSH_CONST: {
        uint64_t bits;
        double v = chunk.constants.at(ins.operand);
        std::memcpy(&bits, &v, sizeof(bits));
        emitter.emit_load_double_to_d0(bits);
        emitter.emit_str_d0_sp_slot(depth);
        depth++;
        break;
      }
      case Op::OP_PUSH_LOCAL:
        if (ins.operand != 0) return nullptr;
        emitter.emit_mov_d0_from_local0();
        emitter.emit_str_d0_sp_slot(depth);
        depth++;
        break;
      case Op::OP_POP:
        depth--;
        break;
      case Op::OP_ADD:
        if (depth < 2) return nullptr;
        emitter.emit_ldr_d0_sp_slot(depth - 2);
        emitter.emit_ldr_d1_sp_slot(depth - 1);
        emitter.emit_fadd_d0_d1();
        emitter.emit_str_d0_sp_slot(depth - 2);
        depth--;
        break;
      case Op::OP_SUB:
        if (depth < 2) return nullptr;
        emitter.emit_ldr_d0_sp_slot(depth - 2);
        emitter.emit_ldr_d1_sp_slot(depth - 1);
        emitter.emit_fsub_d0_d1();
        emitter.emit_str_d0_sp_slot(depth - 2);
        depth--;
        break;
      case Op::OP_MUL:
        if (depth < 2) return nullptr;
        emitter.emit_ldr_d0_sp_slot(depth - 2);
        emitter.emit_ldr_d1_sp_slot(depth - 1);
        emitter.emit_fmul_d0_d1();
        emitter.emit_str_d0_sp_slot(depth - 2);
        depth--;
        break;
      case Op::OP_NEG:
        if (depth < 1) return nullptr;
        emitter.emit_ldr_d0_sp_slot(depth - 1);
        emitter.emit_fneg_d0();
        emitter.emit_str_d0_sp_slot(depth - 1);
        break;
      case Op::OP_LT: {
        if (depth < 2) return nullptr;
        emitter.emit_ldr_d0_sp_slot(depth - 2);
        emitter.emit_ldr_d1_sp_slot(depth - 1);
        emitter.emit_fcmp_d0_with_d1();
        emitter.emit_fcsel_d0_d9_d10_if_lt();
        emitter.emit_str_d0_sp_slot(depth - 2);
        depth--;
        break;
      }
      case Op::OP_JMP:
        fixups.push_back({i, emitter.emit_branch_placeholder(), false});
        break;
      case Op::OP_JMP_IF_FALSE: {
        if (depth < 1) return nullptr;
        emitter.emit_ldr_d0_sp_slot(depth - 1);
        emitter.emit_load_double_to_reg(10, 0);
        emitter.emit_fcmp_d0_d10();
        fixups.push_back({i, emitter.emit_branch_eq_placeholder(), true});
        break;
      }
      case Op::OP_CALL: {
        if (depth < 1) return nullptr;
        emitter.emit_ldr_d0_sp_slot(depth - 1);
        emitter.emit_call_jit_helper(signed_jit_call1(), &vm, ins.operand);
        emitter.emit_str_d0_sp_slot(depth - 1);
        break;
      }
      case Op::OP_RETURN:
        if (depth >= 1) {
          emitter.emit_ldr_d0_sp_slot(depth - 1);
        } else {
          emitter.emit_mov_d0_from_local0();
        }
        emitter.emit_ret();
        break;
      default:
        return nullptr;
    }
  }

  for (const Fixup& f : fixups) {
    const Instruction& ins = chunk.code[f.insn_index];
    std::size_t target_bc = f.insn_index + 1 + static_cast<std::size_t>(ins.operand);
    if (target_bc >= bc_off.size()) return nullptr;
    std::size_t target_native = bc_off[target_bc];
    if (f.is_eq_branch) {
      emitter.patch_branch_eq_to(f.branch_offset, target_native);
    } else {
      emitter.patch_branch_to(f.branch_offset, target_native);
    }
  }

  return CodeCache::publish(emitter.buffer().data(), emitter.size());
#endif
}

}  // namespace tinylang::jit
