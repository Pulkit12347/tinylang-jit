#include "jit/runtime.h"

#if defined(__aarch64__)
#if defined(__APPLE__)
#include <ptrauth.h>
#endif

namespace tinylang::jit {

void* sign_c_function(void* fn) {
#if defined(__APPLE__)
  return ptrauth_sign_unauthenticated(fn, ptrauth_key_function_pointer, 0);
#else
  return fn;
#endif
}

void* signed_jit_call1() {
  static void* p = sign_c_function(reinterpret_cast<void*>(&tinylang_jit_bridge_call1));
  return p;
}

void* signed_jit_bool_lt() {
  static void* p = sign_c_function(reinterpret_cast<void*>(&tinylang_jit_bridge_bool_lt));
  return p;
}

}  // namespace tinylang::jit

#else

namespace tinylang::jit {

void* sign_c_function(void* fn) { return fn; }
void* signed_jit_call1() { return nullptr; }
void* signed_jit_bool_lt() { return nullptr; }

}  // namespace tinylang::jit

#endif
