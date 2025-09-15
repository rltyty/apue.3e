#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // 1. Array initializer must be an initializer list or wide string literal
  // int a[] = calloc(2, sizeof(int));    // ❌ WRONG!

  // 2. Array initialization
  char *ss[] = {
    [0] = "hello",
    [1] = "world",
    [2] = "!",
    [3] = NULL
  };
  for (char **p = ss; *p != NULL; p++) {
    puts(*p);
  }
  assert(sizeof(ss) == sizeof(char*) * 4);

  char *tt[] = {
    [1] = "world",
    [3] = NULL
  };
  for (int i = 0; i < 3; i++) {
    puts(tt[i]);
  }
  assert(sizeof(char*) * 4 == sizeof(tt));

  char *rr[] = {
    "hello", "world", "!", // the dangling comma adds no implicit element
  };
  assert(3 == sizeof(rr)/sizeof(char*));

  return 0;
}

/*

:!Debug/general/test_array
hello
world
!
(null)
world
(null)

 */
