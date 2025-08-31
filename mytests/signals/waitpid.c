#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "rltapue.h"

static const int CHILD_COUNT = 5;
volatile sig_atomic_t live_children = CHILD_COUNT;

void sigchld(int signo) {
  int save_err = errno;
  int stat;
  pid_t pid;
  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
    // printf() is not reentrant, async-signal-safe for use in signal handler
    // just for test in informal way
    printf("sigchld: Reap terminated child: %d\n", pid);
    live_children--;
  }
  errno = save_err;
}

int main(int argc, char *argv[]) {
  // sa_flags = 0, sa_handler / sigaction = NULL, sa_mask = 0
  struct sigaction act_chld = {0};
  sigemptyset(&act_chld.sa_mask); // For portability of sigset_t
  act_chld.sa_handler = sigchld;
  if (sigaction(SIGCHLD, &act_chld, NULL) < 0) {
    my_perror("error: sigaction");
  }


  for (int i = 0; i < CHILD_COUNT; i++) {
    pid_t pid;
    if ((pid = fork()) < 0) {
      my_perror("error: fork()");
    } else if (pid == 0) {
      printf("Child(%d): Enter >>>\n", getpid());
      sleep(i+1);
      printf("Child(%d): Exit <<<\n", getpid());
      exit(0);
    }
  }

  while (live_children > 0) {
    pause();
    printf ("Main: pause() was interrupted and now returned.\n");
  }
  return 0;
}

/**

> Debug/signals/waitpid
Child(78710): Enter >>>
Child(78711): Enter >>>
Child(78712): Enter >>>
Child(78713): Enter >>>
Child(78714): Enter >>>
Child(78710): Exit <<<
sigchld: Reap terminated child: 78710
Main: pause() was interrupted and now returned.
Child(78711): Exit <<<
sigchld: Reap terminated child: 78711
Main: pause() was interrupted and now returned.
Child(78712): Exit <<<
sigchld: Reap terminated child: 78712
Main: pause() was interrupted and now returned.
Child(78713): Exit <<<
sigchld: Reap terminated child: 78713
Main: pause() was interrupted and now returned.
Child(78714): Exit <<<
sigchld: Reap terminated child: 78714
Main: pause() was interrupted and now returned.
>

NOTE:

A. `pause(3)` workflow
1. `pause(3)` is called and the process sleeps.
2. A signal is delivered.
3. The operating system interrupts `pause(3)` and executes the registered
   signal handler.
4. The signal handler completes its execution and returns.
5. *Only then* does the `pause(3)` call, which was interrupted, return to the
   calling code.
If the handler calls `exit()` functions, the process just terminates w/o
returning from `pause(3)` to the calling code.

B. Reentrant and async-signal-safe calls in a signal handler

`printf(3)` is NOT reentrant, because it uses shared internal state:
   - Static/global buffers
   - Locks inside the stdio library
   - Heap allocation for formatting
   - Global variables like errno

Then it is not async-signal-safe. Example:
The main process is in the middle of `printf(3)`. Internally, libc has locked
a mutex on `stdout`'s buffer. Now a child dies, `SIGCHLD` arrives and handler
runs. In the handler, another call of `printf(3)` tries to lock the same mutex.
Since the lock is already held by the interrupted code (in `printf(3)`). Thus
deadlock happens. Besides, same `stdout` buffer can be corrupted.

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
