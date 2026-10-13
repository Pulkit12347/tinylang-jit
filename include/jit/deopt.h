#pragma once

#include <cstdint>

namespace tinylang::jit {

inline uint64_t& safepoint_counter() {
  static uint64_t n = 0;
  return n;
}

inline void record_safepoint() { ++safepoint_counter(); }

inline bool deopt_enabled() { return false; }

}  // namespace tinylang::jit
