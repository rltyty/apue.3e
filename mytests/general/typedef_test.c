#include "rltapue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int IntFunc(int, int); // function pointer definition

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }

int ifunc(IntFunc f, int a, int b) { return f(a, b); } // function pointer
                                                       // variable declaration

int main(int argc, char *argv[]) {
  if (argc < 4)
    my_perror("3 arguments required.");
  int a = atoi(argv[2]);
  int b = atoi(argv[3]);
  if (strcmp("add", argv[1]) == 0) {
    printf("%s(%d, %d)=%d\n", argv[1], a, b, ifunc(add, a, b)); // refer to
  } else if (strcmp("sub", argv[1]) == 0) {
    printf("%s(%d, %d)=%d\n", argv[1], a, b, ifunc(sub, a, b));
  } else {
    printf("Unexpected operation [%s].\n", argv[1]);
  }
  return 0;
}

/*
 * NOTE:
 * 1. Type definition: `typedef int IntFunc(int, int);`
 * 2. Variable declaration: `IntFunc f;`
 * 3. Variable reference: ifunc(f, a, b);
 */
