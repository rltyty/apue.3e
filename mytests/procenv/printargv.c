/* myecho.c */

#include <stdio.h>


int main(int argc, char *argv[]) {
  // // 1.
  // for (int j = 0; j < argc; j++)
  //   printf("argv[%d]: %s\n", j, argv[j]);

  // 2.
  for (int j = 0; argv[j] != NULL; j++)
    printf("argv[%d]: %s\n", j, argv[j]);
}

