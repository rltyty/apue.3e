#include <assert.h>
#include <stdio.h>


/* non-zero: true, 0: false, !0: true, !non-zero: false, !!non-zero: true */

int test_bool(int op) {
  if (op) {
    printf("if (%d) is equivalent to if (true)\n", op);
  } else {
    printf("if (%d) is equivalent to if (false)\n", op);
  }
  return op;
}

void* test_bool2(void* op) {
  if (op) {
    printf("if (%p) is equivalent to if (true)\n", op);
  } else {
    printf("if (%p) is equivalent to if (false)\n", op);
  }
  return op;
}

void test_expression() {
  assert(5 == 5);
  assert(1 == (5 == 5));
  assert(0 == (5 == 5 == 5));     // (5 == 5 == 5) == ((5 == 5) == 5)
}

int main(void) { 

  assert(1);
  assert(!0);
  assert(!!1);
  assert(2);
  assert(-1);
  assert(-2);

  int a[2] = {1, 2}; // initialized automatic
  int b[2];          // uninitialized automatic
  static int c[2];   // static, zero-initialized 
  assert(!NULL);
  assert(a);
  assert(b);
  assert(c);

  assert(!test_bool(0));
  assert(test_bool(1));
  assert(test_bool(2));
  assert(test_bool(-1));
  assert(test_bool(-2));

  assert(!test_bool2(NULL));
  assert(test_bool2(a));
  assert(test_bool2(c));

  return 0;
}
