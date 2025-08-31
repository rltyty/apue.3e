#include <assert.h>

static void lshift(int *p, int shift) {
  (*p) = (*p) << shift;
}

static void rshift(int *p, int shift) {
  (*p) = (*p) >> shift;
}

/**
 * **`__typeof`** is a compiler keyword, not a macro nor a function. It just
 * gives the type of the declared symbol
 * 
 * `const`, `volatile`, `restrict`: they're port of type
 * `static`, `extern`, `auto`, `register`, `inline`: they're NOT part of type,
 * they're storage class or linkage.
 * 
 */

static __typeof(lshift) *shift;

void __typeof_test() {
  int a;
  __typeof(a) b;    // int b;
  assert(sizeof(b) == 4);
  a = 5;
  lshift(&a, 2);
  assert(20 == a);
  b = 20;
  rshift(&b, 2);
  assert(5 == b);

  shift = lshift;
  shift(&b, 2);
  assert(20 == b);

  shift = rshift;
  shift(&a, 2);
  assert(5 == a);
}

int main(int argc, char *argv[]) {

  __typeof_test();

  return 0;
}

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
