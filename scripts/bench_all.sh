#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
make -C "$ROOT" -s

echo "======== add1 (JIT-friendly) ========"
"$ROOT/scripts/bench.sh" "$ROOT/bench/add1.tl"

echo "======== fib (interpreter) ========"
"$ROOT/tinylang" "$ROOT/bench/fib.tl" --interp --bench

echo "======== nbody (interpreter) ========"
"$ROOT/tinylang" "$ROOT/bench/nbody.tl" --interp --bench

if command -v clang >/dev/null; then
  echo "======== fib gcc -O2 ========"
  clang -O2 "$ROOT/bench/c/fib.c" -o /tmp/tinylang-fib-c && /tmp/tinylang-fib-c
  echo "======== nbody gcc -O2 ========"
  clang -O2 "$ROOT/bench/c/nbody.c" -o /tmp/tinylang-nbody-c && /tmp/tinylang-nbody-c
fi
