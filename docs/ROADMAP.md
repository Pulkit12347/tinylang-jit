# Roadmap (discussed vs shipped)

## Shipped

- [x] Bytecode lexer/parser/VM
- [x] Invocation profiling + JIT threshold
- [x] W^X `mmap` code cache
- [x] AArch64 template JIT (stack slots) for **arity-1 arithmetic** (`identity`, `add1`)
- [x] Runtime call helper for nested JIT calls (`tinylang_jit_call1`)
- [x] Benchmarks: `fib`, `add1`, `nbody` (+ C baselines)
- [x] `tinylang_jit_call1` fast path into compiled callees
- [ ] `let` / local reassignment (parser present; VM init bug — use parameters/recursion for now)
- [x] [APPLICATION.md](APPLICATION.md) talking points

## In progress / blocked

- [ ] **Control-flow in JIT** (`LT`, `JMP`, `JMP_IF_FALSE`) — needed for `fib` native code; AArch64 literal calls need a fix (PC-relative pools in JIT buffer)
- [ ] **Direct call-site patching** to compiled entry (today: helper dispatch)

## Planned (not started)

- [ ] Thin LIR + linear-scan regalloc (after safepoints)
- [ ] Safepoints + deopt to interpreter
- [ ] Inline caches ([inline_cache.h](../include/jit/inline_cache.h))
- [ ] OSR for long-running loops
- [ ] x86-64 backend (host uses AArch64 on Apple Silicon)

## How to benchmark today

```bash
make
./scripts/bench.sh bench/add1.tl    # JIT vs interp (JIT wins on tiny kernels)
./tinylang bench/fib.tl --interp --bench
clang -O2 bench/c/fib.c -o /tmp/fib-c && /tmp/fib-c
```
