#include <stdio.h>
#include <unistd.h>
#include "rltapue.h"

int main(int argc, char *argv[]) {
  pid_t pid;

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

# If parent doesn't `wait()` to reap child's exit status, zombie occurs and
# doesn't disappear until the parent dies.

> ./Debug/signals/zombie &
[2] 15062
Parent (PID=15062) pause.
Child (PID=15064) exiting.
> jobs
[1]  + suspended  vim
[2]  - running    ./Debug/signals/zombie
> myps -p 15062,15064
  UID   PID  PPID  PGID   SESS TTY      STAT COMM
  501 15062 49433 15062      0 ttys001  SN   ./Debug/signals/zombie
  501 15064 15062 15062      0 ttys001  ZN   (zombie)
> kill -2 15062
[2]  + 15062 interrupt  ./Debug/signals/zombie
> myps -p 15062,15064
  UID   PID  PPID  PGID   SESS TTY      STAT COMM
 */


// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
