#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace tinylang::jit {

class NativeEmitter {
 public:
  void reset();
  const std::vector<uint8_t>& buffer() const { return bytes_; }
  std::size_t size() const { return bytes_.size(); }

  void emit_u8(uint8_t b);
  void emit_prologue_arity1();
  void emit_epilogue_arity1();
  void emit_ret();

  void emit_str_d0_sp_slot(int slot);
  void emit_ldr_d0_sp_slot(int slot);
  void emit_ldr_d1_sp_slot(int slot);
  void emit_fcmp_d0_zero();
  std::size_t emit_branch_eq_placeholder();
  void patch_branch_eq_to(std::size_t branch_insn_offset, std::size_t target_offset);
  void emit_bl_helper_d0_d1(void* fn_ptr);
  void emit_load_zero_to_d1();

  void emit_load_double_to_d0(uint64_t ieee_bits);
  void emit_load_double_to_reg(int d_reg, uint64_t ieee_bits);
  void emit_fcsel_d0_d2_d3_if_lt();
  void emit_mov_d0_from_local0();
  void emit_mov_d1_from_local0();
  void emit_fadd_d0_d1();
  void emit_fsub_d0_d1();
  void emit_fmul_d0_d1();
  void emit_fdiv_d0_d1();
  void emit_fneg_d0();
  void emit_fcmp_d0_with_d1();

  std::size_t emit_branch_placeholder();
  void patch_branch_to(std::size_t branch_insn_offset, std::size_t target_offset);
  void emit_branch_if_lt(std::size_t branch_insn_offset, std::size_t target_offset);

  void emit_call_jit_helper(void* helper, void* vm, int fn_index);

 private:
  std::vector<uint8_t> bytes_;
  void emit_u32(uint32_t word);
  void emit_literal_u64(uint64_t v);
#if defined(__aarch64__)
  void emit_load_x0(uint64_t ptr);
  void emit_load_x16(uint64_t ptr);
  void emit_mov_w1(int fn_index);
#endif
};

}  // namespace tinylang::jit
