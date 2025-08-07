#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
void test_alloc() {
  int *a = (int*) malloc(sizeof(int)*2);
  assert(sizeof(a) == 8);
  printf("a[0] = %d, a[1] = %d\n", a[0], a[1]);
  free(a);

  int *b = (int*) calloc(2, sizeof(int));
  printf("b[0] = %d, b[1] = %d\n", b[0], b[1]);
  free(b);
}

int main(void) {
  test_alloc();
}

/*
:!Debug/general/test_stdlib
a[0] = 0, a[1] = 0
b[0] = 0, b[1] = 0

:!Release/general/test_stdlib
a[0] = -1085599248, a[1] = -1085599232
b[0] = 0, b[1] = 0

 */
