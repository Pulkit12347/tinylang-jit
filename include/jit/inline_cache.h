#pragma once

// Planned: monomorphic inline caches for dynamic dispatch (obj.method()).
// Language does not expose objects yet; see docs/ROADMAP.md.

namespace tinylang::jit {

struct InlineCache {
  int cached_fn_index = -1;
  void* cached_native = nullptr;
};

}  // namespace tinylang::jit
