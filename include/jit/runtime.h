#pragma once

#include <cstdint>

namespace tinylang {

class VM;

extern "C" double tinylang_jit_call1(VM* vm, int fn_index, double arg0);
extern "C" double tinylang_jit_bool_lt(double a, double b);

#if defined(__aarch64__)
extern "C" void tinylang_jit_bridge_call1(void);
extern "C" void tinylang_jit_bridge_bool_lt(void);
#endif

}  // namespace tinylang
