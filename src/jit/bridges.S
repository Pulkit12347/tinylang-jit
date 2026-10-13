#if defined(__aarch64__)
.text
.align 4
.globl _tinylang_jit_bridge_call1
_tinylang_jit_bridge_call1:
  b _tinylang_jit_call1

.globl _tinylang_jit_bridge_bool_lt
_tinylang_jit_bridge_bool_lt:
  b _tinylang_jit_bool_lt
#endif
