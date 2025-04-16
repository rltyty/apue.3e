#include <time.h>

/* To retrieve original time consumption of code, use Debug build mode,
 * which compiles with -O0 (no optimization) */
double timeit(void (*f)(), int loop) {
  clock_t start, end;
  start = clock();
  for (int i = 0; i < loop; i++)
    f();
  end = clock();
  return ((double)(end - start)) / CLOCKS_PER_SEC;
}

/* Unit test */

#ifdef test_profiling
#include <stdio.h>
void f1() { printf("Hello, world\n"); }
void f2() {
  for (int i = 0; i < 1048576; i++)
    ;
} /* loop 1M times */
int main(void) {
  double t = 0.0;
  t = timeit(f1, 10000000);
  printf("%s loops for %d times takes: %f(s)\n", "f1", 1000, t);
  t = timeit(f2, 10000000);
  printf("%s loops for %d times takes: %f(s)\n", "f2", 1000, t);
}

/*
 * > time ./profiling
 * f1 loops for 1000 times takes: 0.001101(s)
 * f2 loops for 1000 times takes: 1.926505(s)
 * ./profiling  1.92s user 0.01s system 99% cpu 1.944 total
 */
#endif
