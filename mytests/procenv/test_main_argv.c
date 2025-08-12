/* test_main_argv.c */

#include <stdio.h>
#include <stdlib.h>

// argv[0]: program name, argv[1] ~ argv[argc-1]: arguments, argv[argc]: NULL

int main(int argc, char *argv[]) {
  // 1.
  for (int j = 0; j < argc; j++)
    printf("argv[%d]: %s\n", j, argv[j]);
  printf("---\n");

  // 2.
  for (int j = 0; argv[j] != NULL; j++)
    printf("argv[%d]: %s\n", j, argv[j]);
  printf("---\n");

  // 3.
  for (char **p = argv; *p != NULL; p++)
    printf("p: %p, *p: %s\n", p, *p);
  printf("---\n");

  // print argc arguments + 1.
  for (int j = 0; j <= argc; j++)
    printf ("argv[%d]: %s\n", j, argv[j]);
  int r = printf("---\n");

  // implicit return
  printf("printf(\"---\\n\") = %d, but implicit return of main() is 0.\n", r);
}

/*
:!Debug/procenv/test_main_argv a1 a2
argv[1]: a1
argv[2]: a2
---
argv[0]: Debug/procenv/test_main_argv
argv[1]: a1
argv[2]: a2
---
p: 0x7ff7b81901d8, *p: Debug/procenv/test_main_argv
p: 0x7ff7b81901e0, *p: a1
p: 0x7ff7b81901e8, *p: a2
---
argv[0]: Debug/procenv/test_main_argv
argv[1]: a1
argv[2]: a2
argv[3]: (null)
---
 */
