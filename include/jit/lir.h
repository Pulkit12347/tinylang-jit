#pragma once

#include "bytecode.h"

// Thin LIR (non-SSA): shared bytecode subset check before template JIT / future regalloc.

namespace tinylang::jit {

enum class LirOp { Mov, Add, Call, Branch, Ret };

bool bytecode_supported_by_lir(const Chunk& chunk);

}  // namespace tinylang::jit
