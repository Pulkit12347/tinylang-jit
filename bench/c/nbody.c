#include <stdio.h>
#include <time.h>

static double steps(int n, int pi) {
  double sum = 0.0;
  for (int i = 0; i < n; i++) {
    double acc = 0.0;
    for (int j = 0; j < pi; j++) {
      acc += 0.0001;
    }
    sum += acc;
  }
  return sum;
}

int main(void) {
  clock_t t0 = clock();
  double r = steps(200, 200);
  clock_t t1 = clock();
  printf("%g\n", r);
  printf("time_ms=%.3f\n", 1000.0 * (t1 - t0) / CLOCKS_PER_SEC);
  return 0;
}
