#include "rltapue.h"
#include <stdlib.h>
#include <unistd.h>
int main(void) {
  if (execl("./tmp/data/procenv/interpreter.file",
            "interpreter.file", "arg1", "arg2", (char*)0) < 0)
    my_perror("execl error");
  exit(0);
}

/*
> cat ./tmp/data/procenv/interpreter.file
#! Debug/procenv/myecho myecho_arg1 myecho_arg2

> Debug/procenv/execl
argv[0]: Debug/procenv/myecho
argv[1]: myecho_arg1
argv[2]: myecho_arg2
argv[3]: ./tmp/data/procenv/interpreter.file
argv[4]: arg1
argv[5]: arg2

NOTE:

An interpreter file
```sh
#! pathname [ optional-argument ]
```
The `pathname` after `#!` is the **interpreter**.

1. The kernel will locate and load the interpreter from the pathname in
   the file.
2. The `argv[0]` passed to the interpreter is the interpreter path itself.
3. the remaining arguments are passed in the below order:
   3.1 the optional argument after the interpreter path
   3.2 the path passed to `exec`, i.e. the interpreter file path
   3.3 argv[1], argv[2], ... of the `exec` call (argv[0] and the NULL excluded)

*/
