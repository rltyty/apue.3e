#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "rltapue.h"

int main(int argc, char *argv[]) {
  pid_t pid;

  signal(SIGCHLD, SIG_IGN); // no zombie if no wait for reaping

  if ((pid = fork()) < 0) {
    my_perror("error: fork");
  } else if (pid == 0) {
    printf("Child (PID=%d) exiting.\n", getpid());
    return 0;
  } else {
    printf("Parent (PID=%d) pause.\n", getpid());
    while(1) pause();
  }
  return 0;
}

/*

# Set SIG_IGN to SIGCHLD, The kernel automatically reaps child processes
# creating zombies.

```
> ./Debug/signals/zombie2 &
[2] 18593
Parent (PID=18593) pause.
Child (PID=18595) exiting.
> jobs
[1]  + suspended  vim
[2]    running    ./Debug/signals/zombie2
> myps -p 18593,18595
  UID   PID  PPID  PGID   SESS TTY      STAT COMM
  501 18593 49433 18593      0 ttys001  SN   ./Debug/signals/zombie2
> kill -2 18593
[2]    18593 interrupt  ./Debug/signals/zombie2
> jobs
[1]  + suspended  vim
```
*/
// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
