/* execve.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  static char *newargv[] = {"this will be overridden", "hello", "world", NULL};
  static char *newenviron[] = {NULL};

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file-to-exec>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  execve(argv[1], newargv, newenviron);
  perror("execve"); /* execve() returns only on error */
  exit(EXIT_FAILURE);
}

/*
 *
cat ./tmp/data/procenv/interpreter.file
#! Debug/procenv/printargv interp_arg1 interp_arg2

Debug/procenv/execve ./tmp/data/procenv/interpreter.file
argv[0]: Debug/procenv/printargv
argv[1]: interp_arg1
argv[2]: interp_arg2
argv[3]: ./tmp/data/procenv/interpreter.file    <-- overridden by kernel
argv[4]: hello
argv[5]: world

 */
