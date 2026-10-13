#pragma once

// On-stack replacement: compile hot loops inside long-running interpreter frames.
// Planned once we have LIR + frame maps at loop headers (`while` in bytecode).

namespace tinylang::jit {

inline bool osr_enabled() { return false; }

}  // namespace tinylang::jit
