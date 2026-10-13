#pragma once

// Polymorphic call-site cache (up to 2 targets) keyed by (caller fn, call insn).

namespace tinylang::jit {

struct InlineCache {
  static constexpr int kMax = 2;
  int count = 0;
  int fn_index[kMax] = {-1, -1};
  void* native[kMax] = {nullptr, nullptr};
};

}  // namespace tinylang::jit
