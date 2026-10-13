#pragma once

// Thin LIR (non-SSA) placeholder for future regalloc / OSR / deopt.
// v1 compiles bytecode -> AArch64 directly (template JIT).

namespace tinylang::jit {

enum class LirOp { Mov, Add, Call, Branch, Ret };

}  // namespace tinylang::jit
