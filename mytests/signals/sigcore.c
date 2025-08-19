#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include "rltapue.h"

int main(int argc, char *argv[]) {
  pid_t pid;

  if ((pid = fork()) < 0) {
    my_perror("error: fork");
  } else if (pid == 0) { // child
    while(1) pause();
    _exit(1);
  } else { // parent
    sleep(3);
    printf("I'm a parent (PID:%d), I have a child (PID:%d)\n\n", getpid(), pid);
    int rc;
    waitpid(pid, &rc, WUNTRACED);
    pr_statsig(rc);
  }

  return 0;
}

/*

> ulimit -c
0
> ulimit -c unlimited

# Tells a process to generate core dump

> ./Debug/signals/sigcore &
[2] 53132
> I'm a parent (PID:53132), I have a child (PID:53135)
>
> jobs
[1]  + suspended  vim
[2]  - running    ./Debug/signals/sigcore
> myps -p 53132,53135
  UID   PID  PPID  PGID   SESS TTY      STAT COMM
  501 53132 49433 53132      0 ttys001  SN   ./Debug/signals/sigcore
  501 53135 53132 53132      0 ttys001  SN   ./Debug/signals/sigcore

> kill -SIGABRT 53135

> myps -p 53132,53135
  UID   PID  PPID  PGID   SESS TTY      STAT COMM
  501 53132 49433 53132      0 ttys001  SN   ./Debug/signals/sigcore
  501 53135 53132 53132      0 ttys001  R    ./Debug/signals/sigcore
> Child killed by signal 6 (Abort trap: 6)

[2]  - 53132 done       ./Debug/signals/sigcore

> myps -p 53132,53135
  UID   PID  PPID  PGID   SESS TTY      STAT COMM

> ll -h /cores
total 3.6G
drwxrwxrwt  3 root   wheel   96 Aug 18 19:36 ./
drwxr-xr-x 20 root   wheel  640 Jul 17  2024 ../
-r--------  1 scv wheel 3.6G Aug 18 19:36 core.53135

# Tells a process group to generate core dump

> ./Debug/signals/sigcore &
[2] 62114
> I'm a parent (PID:62114), I have a child (PID:62117)
>
> kill -SIGABRT -62114
>
[2]    62114 abort (core dumped)  ./Debug/signals/sigcore
> ll -h /cores
total 7.1G
drwxrwxrwt  4 root   wheel  128 Aug 18 19:54 ./
drwxr-xr-x 20 root   wheel  640 Jul 17  2024 ../
-r--------  1 gpanda wheel 3.6G Aug 18 19:54 core.62114
-r--------  1 gpanda wheel 3.6G Aug 18 19:54 core.62117


# Tells a job to generate core dump

> ./Debug/signals/sigcore &
[2] 59032
> I'm a parent (PID:59032), I have a child (PID:59035)
>
> kill -SIGABRT %2
>
[2]    59032 abort (core dumped)  ./Debug/signals/sigcore

> ll -h /cores/
total 7.1G
drwxrwxrwt  4 root   wheel  128 Aug 18 19:48 ./
drwxr-xr-x 20 root   wheel  640 Jul 17  2024 ../
-r--------  1 gpanda wheel 3.6G Aug 18 19:48 core.59032
-r--------  1 gpanda wheel 3.6G Aug 18 19:48 core.59035

 */



// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
