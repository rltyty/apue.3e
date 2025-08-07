/* myecho.c */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  for (size_t j = 0; j < argc; j++)
    printf("argv[%zu]: %s\n", j, argv[j]);
  system("date"); // implemented by calling fork(2), execve(2), waitpid(2)
  exit(EXIT_SUCCESS);
}
