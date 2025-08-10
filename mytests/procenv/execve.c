/* execve.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  static char *newargv[] = {NULL, "hello", "world", NULL};
  static char *newenviron[] = {NULL};

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file-to-exec>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  newargv[0] = argv[1];

  execve(argv[1], newargv, newenviron);
  perror("execve"); /* execve() returns only on error */
  exit(EXIT_FAILURE);
}

/*
 *
cat ./tmp/data/procenv/interpreter.file
#! Debug/procenv/myecho myecho_arg1 myecho_arg2

Debug/procenv/execve ./tmp/data/procenv/interpreter.file
Sun Aug 10 10:48:13 CST 2025
argv[0]: Debug/procenv/myecho
argv[1]: myecho_arg1
argv[2]: myecho_arg2
argv[3]: ./tmp/data/procenv/interpreter.file
argv[4]: hello
argv[5]: world

 */
