# tinylang-jit

A small dynamic-ish language with a bytecode interpreter and a tiered JIT.

## Pipeline

```
source (.tl) → parser → bytecode → interpreter (tier 0)
                              ↘ profiling counters
                                ↘ template JIT → mmap'd executable code (tier 1)
```

## Build & run

```bash
make
./tinylang bench/add1.tl --jit --jit-threshold 4
./tinylang bench/call_inc.tl --jit --jit-threshold 4   # JIT call to another function
./tinylang bench/fib.tl --interp --bench               # fib uses interpreter (no self-CALL JIT yet)
./scripts/bench_all.sh
```

## Flags

- `--interp` / `--jit` / `--jit-threshold N` / `--bench` / `--disasm`

## JIT status (AArch64)

| Feature | Status |
|---------|--------|
| Arity-1 arithmetic (`add1`, `identity`) | JIT |
| `LT` as expression (`lt_only`) | JIT |
| `CALL` to **other** functions (`call_inc`) | JIT via runtime bridge |
| `if` / `JMP_IF_FALSE` | Interpreter recommended |
| Self-recursive `CALL` (`fib`) | Interpreter |

## Docs

- [docs/ROADMAP.md](docs/ROADMAP.md)
- [docs/APPLICATION.md](docs/APPLICATION.md)
