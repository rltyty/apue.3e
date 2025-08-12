#include "rltapue.h"
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  if (execl("./tmp/data/procenv/interpreter.file",
            "this will be overridden", "arg1", "arg2", (char*)0) < 0)
    my_perror("execl error");
  exit(0);
}

/*
> cat ./tmp/data/procenv/interpreter.file
#! Debug/procenv/printargv interp_arg1 interp_arg2

:!Debug/procenv/execl
argv[0]: Debug/procenv/printargv
argv[1]: interp_arg1
argv[2]: interp_arg2
argv[3]: ./tmp/data/procenv/interpreter.file    <-- overridden by kernel
argv[4]: arg1
argv[5]: arg2

Read the man page `execve(2)`

If the first two bytes of a script are `#!` (***shebang***), and they're
followed by an **interpreter**, it is an interpreter file (***shebang***
script).

```sh
#! interpreter [ optional-argument ...]
...
```

When it is executed via `./a.sh` or `execve(interpreter_file_path, argv, envp)`

1. The kernel locates and load the **interpreter** after the ***shebang***
2. The arguments to be passed to **interpreter**  are built in this order:
   - the zeroth argument: the **interpreter** itself
   - the first (second, ..) argument: the optional arguments after the **interpreter**
   - the script file path (or the path passed to `execve()`)
   - the original arguments passed to the script or `exec` call (`argv`)
3. The kernel calls a second `execve()` with the **interpreter** and arguments.
   - if the **interpreter** is an executable binary, the kernel will setup
     process image and jump to `_start` in user process.
   - if the **interpreter** is also an ***shebang*** script, the kernel goes
     back to step 1, recursively. (Linux: BINPRM_MAX_RECURSION=4, if exceeded,
     failed with `ELOOP`)

**NOTE:**
  a. The kernel overrides the first argument passed to the ***shebang***
     script with the script path.
  b. On Linux the kernel only supports one optional shebang argument -- it's
     just one contiguous string after the interpreter path, up to newline
     script 
     argv[0]: Debug/procenv/printargv
     argv[1]: interp_arg1 interp_arg2   <- Linux: the rest of the line after the interpreter
     argv[2]: ./tmp/data/procenv/interpreter.file
     ...
*/
