#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/tinylang"
FILE="${1:-$ROOT/bench/add1.tl}"

make -C "$ROOT" -s

echo "=== interpreter ==="
"$BIN" "$FILE" --interp --bench
echo "=== JIT (threshold 8) ==="
"$BIN" "$FILE" --jit --jit-threshold 8 --bench
