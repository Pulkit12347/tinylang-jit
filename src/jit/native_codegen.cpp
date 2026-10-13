#include "jit/native_codegen.h"

#include "jit/runtime_helpers.h"

#include <cstring>

namespace tinylang::jit {

void NativeEmitter::reset() { bytes_.clear(); }

void NativeEmitter::emit_u8(uint8_t b) { bytes_.push_back(b); }

void NativeEmitter::emit_u32(uint32_t word) {
  emit_u8(static_cast<uint8_t>(word & 0xff));
  emit_u8(static_cast<uint8_t>((word >> 8) & 0xff));
  emit_u8(static_cast<uint8_t>((word >> 16) & 0xff));
  emit_u8(static_cast<uint8_t>((word >> 24) & 0xff));
}

void NativeEmitter::emit_literal_u64(uint64_t v) {
  for (int i = 0; i < 8; ++i) emit_u8(static_cast<uint8_t>((v >> (i * 8)) & 0xff));
}

#if defined(__aarch64__)

void NativeEmitter::emit_prologue_arity1() {
  emit_u32(0xa9bf7bfd);  // stp x29, x30, [sp, #-16]!
  emit_u32(0x910003fd);  // mov x29, sp
  emit_u32(0xd10203ff);  // sub sp, sp, #128 (16 stack slots)
  emit_u32(0x1e604008);  // fmov d8, d0 (param in d8)
  emit_load_double_to_reg(9, 0x3ff0000000000000ull);  // d9 = 1.0 (truth / LT)
  emit_load_double_to_reg(10, 0);                     // d10 = 0.0
}

void NativeEmitter::emit_str_d0_sp_slot(int slot) {
  uint32_t imm12 = static_cast<uint32_t>(slot);
  emit_u32(0xfd0003e0u | (imm12 << 10));  // str d0, [sp, #slot*8]
}

void NativeEmitter::emit_ldr_d0_sp_slot(int slot) {
  uint32_t imm12 = static_cast<uint32_t>(slot);
  emit_u32(0xfd4003e0u | (imm12 << 10));  // ldr d0, [sp, #slot*8]
}

void NativeEmitter::emit_ldr_d1_sp_slot(int slot) {
  uint32_t imm12 = static_cast<uint32_t>(slot);
  emit_u32(0xfd4003e1u | (imm12 << 10));  // ldr d1, [sp, #slot*8]
}

void NativeEmitter::emit_load_zero_to_d1() {
  emit_u32(0x58000040);  // ldr x0, #8
  emit_u32(0x14000003);  // b +12
  emit_literal_u64(0);
  emit_u32(0x9e670001);  // fmov d1, x0
}

void NativeEmitter::emit_fcmp_d0_zero() {
  emit_load_zero_to_d1();
  emit_fcmp_d0_with_d1();
}

std::size_t NativeEmitter::emit_branch_eq_placeholder() {
  std::size_t at = size();
  emit_u32(0x54000001);  // b.eq #0 (patched)
  return at;
}

void NativeEmitter::patch_branch_eq_to(std::size_t branch_insn_offset, std::size_t target_offset) {
  int32_t imm19 = static_cast<int32_t>((target_offset - branch_insn_offset) / 4);
  uint32_t insn = 0x54000001u | ((static_cast<uint32_t>(imm19) & 0x7ffffu) << 5);
  std::memcpy(bytes_.data() + branch_insn_offset, &insn, 4);
}

void NativeEmitter::emit_call_fn_d0_d1(void* fn_ptr) {
  emit_u32(0xf81f0fe8);  // str d8, [sp, #-16]!
  void* target = sign_c_function(fn_ptr);
  emit_load_x16(reinterpret_cast<uint64_t>(target));
  emit_u32(0xd63f0200);  // blr x16
  emit_u32(0xf84107e8);  // ldr d8, [sp], #16
}

void NativeEmitter::emit_fcsel_d0_d2_d3_if_lt() {
  emit_u32(0x1e634c40);  // fcsel d0, d2, d3, mi
}

void NativeEmitter::emit_fcsel_d0_d9_d10_if_lt() {
  emit_u32(0x1e6a4d20);  // fcsel d0, d9, d10, mi
}

void NativeEmitter::emit_fcmp_d0_d10() { emit_u32(0x1e6a2000); }  // fcmp d0, d10

void NativeEmitter::emit_epilogue_arity1() {
  emit_u32(0x910203ff);  // add sp, sp, #128
  emit_u32(0xa8c17bfd);
  emit_u32(0xd65f03c0);
}

void NativeEmitter::emit_ret() { emit_epilogue_arity1(); }

void NativeEmitter::emit_load_double_to_reg(int d_reg, uint64_t ieee_bits) {
  emit_u32(0x58000040);
  emit_u32(0x14000003);
  emit_literal_u64(ieee_bits);
  emit_u32(0x9e670000u | static_cast<uint32_t>(d_reg));  // fmov dN, x0
}

void NativeEmitter::emit_load_double_to_d0(uint64_t ieee_bits) { emit_load_double_to_reg(0, ieee_bits); }

void NativeEmitter::emit_mov_d0_from_local0() { emit_u32(0x1e604100); }  // fmov d0, d8
void NativeEmitter::emit_mov_d1_from_local0() { emit_u32(0x1e604120); }  // fmov d1, d8
void NativeEmitter::emit_fadd_d0_d1() { emit_u32(0x1e612800); }
void NativeEmitter::emit_fsub_d0_d1() { emit_u32(0x1e613800); }
void NativeEmitter::emit_fmul_d0_d1() { emit_u32(0x1e610800); }
void NativeEmitter::emit_fdiv_d0_d1() { emit_u32(0x1e611800); }
void NativeEmitter::emit_fneg_d0() { emit_u32(0x1e614000); }
void NativeEmitter::emit_fcmp_d0_with_d1() { emit_u32(0x1e612000); }

std::size_t NativeEmitter::emit_branch_placeholder() {
  std::size_t at = size();
  emit_u32(0x14000000);
  return at;
}

void NativeEmitter::patch_branch_to(std::size_t branch_insn_offset, std::size_t target_offset) {
  int32_t imm26 = static_cast<int32_t>((target_offset - branch_insn_offset) / 4);
  uint32_t insn = 0x14000000u | (static_cast<uint32_t>(imm26) & 0x03ffffffu);
  std::memcpy(bytes_.data() + branch_insn_offset, &insn, 4);
}

void NativeEmitter::emit_branch_if_lt(std::size_t branch_insn_offset, std::size_t target_offset) {
  int32_t imm19 = static_cast<int32_t>((target_offset - branch_insn_offset) / 4);
  uint32_t insn = 0x5400000au | ((static_cast<uint32_t>(imm19) & 0x7ffffu) << 5);
  std::memcpy(bytes_.data() + branch_insn_offset, &insn, 4);
}

void NativeEmitter::emit_load_x0(uint64_t ptr) {
  emit_u32(0x58000040);  // ldr x0, #8
  emit_u32(0x14000003);  // b +12 (skip 8-byte literal)
  emit_literal_u64(ptr);
}

void NativeEmitter::emit_load_x16(uint64_t ptr) {
  emit_u32(0x58000050);  // ldr x16, #8
  emit_u32(0x14000003);  // b +12
  emit_literal_u64(ptr);
}

void NativeEmitter::emit_mov_x1(int fn_index) {
  uint32_t imm = static_cast<uint32_t>(fn_index) & 0xffffu;
  emit_u32(0xd2800001u | (imm << 5));  // mov x1, #imm
}

void NativeEmitter::emit_call_jit_helper(void* helper, void* vm, int fn_index) {
  emit_u32(0xf81f0fe8);  // str d8, [sp, #-16]!

  emit_load_x0(reinterpret_cast<uint64_t>(vm));
  emit_mov_x1(fn_index);
  // d0 already holds argument

  emit_load_x16(reinterpret_cast<uint64_t>(sign_c_function(helper)));
  emit_u32(0xd63f0200);  // blr x16

  emit_u32(0xf84107e8);  // ldr d8, [sp], #16
}

#else

void NativeEmitter::emit_prologue_arity1() {}
void NativeEmitter::emit_epilogue_arity1() {}
void NativeEmitter::emit_ret() {}
void NativeEmitter::emit_load_double_to_d0(uint64_t) {}
void NativeEmitter::emit_mov_d0_from_local0() {}
void NativeEmitter::emit_mov_d1_from_local0() {}
void NativeEmitter::emit_fadd_d0_d1() {}
void NativeEmitter::emit_fsub_d0_d1() {}
void NativeEmitter::emit_fmul_d0_d1() {}
void NativeEmitter::emit_fdiv_d0_d1() {}
void NativeEmitter::emit_fneg_d0() {}
void NativeEmitter::emit_fcmp_d0_with_d1() {}
void NativeEmitter::emit_fcsel_d0_d9_d10_if_lt() {}
void NativeEmitter::emit_fcmp_d0_d10() {}
std::size_t NativeEmitter::emit_branch_placeholder() { return 0; }
void NativeEmitter::patch_branch_to(std::size_t, std::size_t) {}
void NativeEmitter::emit_branch_if_lt(std::size_t, std::size_t) {}
void NativeEmitter::emit_call_jit_helper(void*, void*, int) {}

#endif

}  // namespace tinylang::jit
