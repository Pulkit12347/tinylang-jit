# Roadmap (discussed vs shipped)

## Shipped

- [x] Bytecode lexer/parser/VM + `let` / locals on `CALL`
- [x] Invocation profiling + JIT threshold
- [x] Per-function W^X `mmap` (`CodeCache::publish`)
- [x] AArch64 template JIT: arithmetic, `LT`, branches, **non-recursive `CALL`**
- [x] Text-section **bridges** + ptrauth-signed targets for JIT → runtime calls
- [x] Polymorphic (2-way) call-site inline caches + safepoint counter (`deopt.h`)
- [x] Thin LIR subset gate (`lir.cpp`)
- [x] OSR stub (`osr.h`)
- [x] Benchmarks + `scripts/bench_all.sh`
- [x] `--disasm`

## In progress

- [ ] **Self-recursive JIT** (`fib` with `CALL` to self) — interpreter today; direct recursion not compiled
- [ ] **x86-64** backend (stubs in `native_codegen.cpp`)

## Planned (stretch)

- [ ] Linear-scan regalloc on LIR (after real safepoint/deopt frames)
- [ ] Full deopt to interpreter at safepoints
- [ ] OSR at loop headers
- [ ] Wider polymorphic ICs

## How to benchmark today

```bash
make
./scripts/bench_all.sh
./tinylang bench/call_inc.tl --jit --jit-threshold 4
./tinylang bench/fib.tl --interp --bench
```
