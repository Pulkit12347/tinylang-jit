# Talking points for applications (CMU / Stanford / MIT-style)

## One-line summary

Built **tinylang-jit**: a small language with a bytecode VM and a profiling JIT that compiles hot functions to native AArch64 in an `mmap`'d W^X code region.

## What to emphasize

- **Problem:** Dynamic languages are slow in an interpreter; production VMs use tiered JITs.
- **Approach:** Bytecode interpreter baseline → invocation counters → template machine-code emitter for hot arity-1 arithmetic functions (`add1`); recursive/`fib`-style control flow still uses the interpreter until branch JIT lands.
- **Technical depth:** Parser/VM, calling convention, `mmap` + `mprotect`, branch patching, C helpers for calls and comparisons.
- **Honesty:** No SSA/OSR yet; spill-to-stack template JIT; compare **interp vs JIT**, not vs `gcc -O2` on numeric kernels.

## Suggested activities bullet (150 chars)

```
Built tinylang-jit: bytecode VM + profiling JIT to AArch64; mmap W^X code cache; fib JIT faster than interpreter at --bench
```

## How to reproduce numbers

```bash
make
./scripts/bench.sh bench/fib.tl
```

## What’s next (if asked)

Inline caches for dynamic dispatch, safepoints/deopt, linear-scan regalloc, OSR for hot loops.
