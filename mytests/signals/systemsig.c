#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rltapue.h"

/**
 `system(3)` ignores SIGINT and SIGQUIT, blocks SIGCHLD
 */

void sig_hand(int signo) {
  printf("Caught signal[%d]=%s\n", signo, strsignal(signo));
}

int main(int argc, char *argv[]) {
  struct sigaction act = {0};
  sigemptyset(&act.sa_mask);
  act.sa_handler = sig_hand;
  if (sigaction(SIGINT, &act, NULL) < 0) {
    my_perror("error: sigaction: %d", SIGINT);
  }
  if (sigaction(SIGCHLD, &act, NULL) < 0) {
    my_perror("error: sigaction: %d", SIGCHLD);
  }
  if (sigaction(SIGUSR1, &act, NULL) < 0) {
    my_perror("error: sigaction: %d", SIGUSR1);
  }
  if (system("/bin/ed") < 0) {
    my_perror("error: system()");
  }

  pause();
  return 0;
}

/*

# Terminal A:
> Debug/signals/systemsig
a
hello
^C                                            <-- SIGINT only received by child
?                                             <-- and interrupts `ed` input
你好
?                                             <-- `ed` is in command mode
a                                             <-- needs `a` to resume appending
你好
Caught signal[30]=User defined signal 1: 30   <-- kill -USR1 89613, received by
parent Ciao
.
w out
18
q
^CCaught signal[2]=Interrupt: 2               <-- <CTRL-C> to interrupt parent
pause() > cat out hello 你好 Ciao
>

# Terminal B:
> pgrep systemsig
89613
> kill -USR1 89613                            <-- caught by parent
> kill -INT 89613                             <-- ignored by parent
> kill -QUIT 89613                            <-- ignored by parent
> kill -CHLD 89613                            <-- blocked by parent
>

NOTE:
The above example illustrates:
1. In system(), the parent ignores SIGQUIT and SIGINT (via `sigaction(2)` and
   `(struct sigaction).sa_handler`). But the child running program via
   `exec()`, respects both.
2. In system(), parent blocks SIGCHLD (via `sigprocmask(2)`). When `ed` is
   running, received `SIGCHLD` is at pending state. When `ed` quits, the kernel
   sends another `SIGCHLD` to the parent. Standard (non-RT) signals are
   not queued, so two `SIGCHLD` collapse into one. And when `waitpid(2)` reaps
   the terminated child (`ed`), the kernel clears the pending `SIGCHLD` bit.
   Thus, even if `SIGCHLD` gets unblocked before `system()` returns, no active
   pending `SIGCHLD` left for the pause(). It keeps blocking until new signal
   arrives.
3. It needs `a` command for `ed` to resume appending mode after interruption.
4. In UTF-8 encoding, each Chinese character consists of 3 bytes.

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
