/* exec_executable.c */

#include <unistd.h>
#include "rltapue.h"

int main(int argc, char *argv[]) {
  // exec: executable
  if (execl("./Debug/procenv/printargv", "this will NOT be overridden", "arg1", "arg2", (char*)0) < 0)
    my_perror("execl error");
  return 0;
}

/*
Debug/procenv/exec_executable
argv[0]: this will NOT be overridden
argv[1]: arg1
argv[2]: arg2
 */
