// Interpreter benchmark (recursive counter; no `let` reassignment)
fn work(n, acc) {
  if (n < 1) {
    return acc
  }
  return work(n - 1, acc + 0.0001)
}

print(work(40000, 0))
