#include "jit/lir.h"

#include "bytecode.h"

namespace tinylang::jit {

// Thin LIR: documents the lowering path for future linear-scan / OSR work.
// Today the template JIT reads bytecode directly; this validates the same subset.
bool bytecode_supported_by_lir(const Chunk& chunk) {
  if (chunk.arity != 1) return false;
  for (const Instruction& ins : chunk.code) {
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

}  // namespace tinylang::jit
