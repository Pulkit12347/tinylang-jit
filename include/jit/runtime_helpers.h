#pragma once

#if defined(__aarch64__)

namespace tinylang::jit {

void* sign_c_function(void* fn);
void* signed_jit_call1();
void* signed_jit_bool_lt();

}  // namespace tinylang::jit

#endif
