/* myecho.c */

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
  for (int j = 0; j < argc; j++)
    printf("argv[%d]: %s\n", j, argv[j]);

  // fflush(stdout);  // explicitly force it out

  // forks here
  system("date"); // implemented by calling fork(2), execve(2), waitpid(2)
  exit(EXIT_SUCCESS);
}

/*
- Run in Neovim:
:!Debug/procenv/myecho a1 a2
Sun Aug 10 11:41:32 CST 2025        <--
argv[0]: Debug/procenv/myecho
argv[1]: a1
argv[2]: a2

Run in Console:
> mytests/Debug/procenv/myecho a1 a2
argv[0]: mytests/Debug/procenv/myecho
argv[1]: a1
argv[2]: a2
Sun Aug 10 12:25:18 CST 2025        <--

NOTE:
- system("date")
  - fork(2), exec(2)
  - new child process from fork(2) inherits a copy of FD table
  - exec(2) replace the child process image with new image for `date`
  - new child process `date` has its own new user space, new
    `stdout` (FILE stream) and its buffer managed by C standard library.
  - kernel space structures like per-process FD table, kernel-wide file table
    survive `exec(2)`, `date`'s `stdout` use the same `STDOUT_FILENO` and
    writes to the same file (Neovim pipe) as its parent does.
- Conditions of stdout flushing out being triggered:
  - Newline ('\n') is printed while connected to line buffered terminal
  - The buffer becomes full (fully buffer mode).
  - Explicitly flush out by `fflush(stdout);`
  - exit()` is called explicitly or automatically when `main()` returns.
  - fclose(stdout)`
- The discrepancy of output order shown above is because the C library
  (printf(3), puts(3), etc.) chooses buffering mode for `stdout` based on what
  it's connected to.
  - When run in Neovim command line, the stdout is connected to a pipe,
    created by Neovim to capture commands output and then write them to
    command area, the buffer is fully buffered, trigger by buffer full or
    explicit `fflush`.
  - When run in Console, the `stdout` is connected to terminal (TTY), the
    buffer mode is line-buffered, flush on every `\n`
  So `printf(3)` in the main process wait for buffer being full or when the
  child exit(), the main process `stdout` flushes when `exit(3)` returns.
 */


