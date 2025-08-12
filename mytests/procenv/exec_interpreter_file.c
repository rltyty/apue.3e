/* exec_interpreter_file.c */

#include <unistd.h>
#include "rltapue.h"

int main(int argc, char *argv[]) {
  // exec: shebang interpreter file
  if (execl("./tmp/data/procenv/interpreter.file", "this will be overridden", "arg1", "arg2", (char*)0) < 0)
    my_perror("execl error");
  return 0;
}

/*
cat ./tmp/data/procenv/interpreter.file
#! Debug/procenv/printargv interp_arg1 interp_arg2

Unix (macOS):
:!Debug/procenv/exec_interpreter_file
argv[0]: Debug/procenv/printargv
argv[1]: interp_arg1
argv[2]: interp_arg2
argv[3]: ./tmp/data/procenv/interpreter.file
argv[4]: arg1
argv[5]: arg2

Linux:
:!Debug/procenv/exec_interpreter_file
argv[0]: Debug/procenv/printargv
argv[1]: interp_arg1 interp_arg2        <- Linux: the rest of the line after the interpreter
argv[2]: ./tmp/data/procenv/interpreter.file
argv[3]: arg1
argv[4]: arg2

 */
