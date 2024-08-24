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

int main(void) { 

  assert(1);
  assert(!0);
  assert(!!1);
  assert(2);
  assert(-1);
  assert(-2);

  assert(!test_bool(0));
  assert(test_bool(1));
  assert(test_bool(2));
  assert(test_bool(-1));
  assert(test_bool(-2));

  return 0;
}
