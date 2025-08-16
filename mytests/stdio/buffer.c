#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
  printf("Main process says Hi!\n");
  system("date");
  return 0;
}

/* Sample Output:

'\n' only triggers lined-buffered stream and a stream is a lined-buffered only
when it connects to a terminal (TTY).

In Neovim:

:!Debug/stdio/buffer
Sat Aug 16 12:14:32 CST 2025
Main process says Hi!

In terminal:

> ./Debug/stdio/buffer
Main process says Hi!
Sat Aug 16 12:15:16 CST 2025

Redirect to a regular file (log) in terminal:

> ./Debug/stdio/buffer > log
> cat log
Sat Aug 16 12:16:05 CST 2025
Main process says Hi!

Redirect to a regular file (log) in Neovim:

:!./Debug/stdio/buffer > log2
Sat Aug 16 12:16:54 CST 2025
Main process says Hi!

 */
