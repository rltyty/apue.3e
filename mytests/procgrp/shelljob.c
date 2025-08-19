#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rltapue.h"

int main(int argc, char *argv[]) {
  pid_t pid;

  if ((pid = fork()) < 0) {
    my_perror("error: fork");
  } else if (pid == 0) {  // child
    while (1) pause();
    _exit(1);
  } else {  // parent
    sleep(3);
    printf("I'm a parent (PID:%d), I have a child (PID:%d)\n\n", getpid(), pid);
    int rc;
    while(waitpid(pid, &rc, WUNTRACED|WCONTINUED) > 0) {
      pr_statsig(rc);
    };
  }

  return 0;
}

/*

----------------------------------------------------------------------
Test 1: Run program in foreground. In a controlling terminal, send a
terminal-generated signal SIGINT, by pressing <CTRL-C>, to the foreground
job (a process group). Both parent and child processes die.
----------------------------------------------------------------------
> ./Debug/procgrp/shelljob
I'm a parent (PID:57742), I have a child (PID:57743)

^C
> myps -p 57742,57743
  UID   PID  PPID  PGID   SESS TTY      STAT COMM

Before press ^C, ps info in another terminal shows that the ID of
the foreground process group (PGID) and its leader is the parent process.

> myps -p 57742,57743
  UID   PID  PPID  PGID   SESS TTY      STAT COMM
  501 57742 49433 57742      0 ttys001  S+   ./Debug/procgrp/shelljob
  501 57743 57742 57742      0 ttys001  S+   ./Debug/procgrp/shelljob

^C makes the terminal send SIGINT to the foreground job (process group).
Like,

> kill -2 -57742 # or kill -INT -57742

This is different from

> kill -2 57742  (PID)

which only kills the parent, the child is still alive.


----------------------------------------------------------------------
Test 2: Run program in foreground. Send SIGTSTP, by pressing <CTRL-Z>,
to the foreground job, to make it become a background suspended job.
----------------------------------------------------------------------
> ./Debug/procgrp/shelljob
I'm a parent (PID:5219), I have a child (PID:5220)

^Z
[2]  + 5219 suspended
> jobs
[1]  - suspended  vim
[2]  + suspended  ./Debug/procgrp/shelljob
> myps -p 5219,5220
  UID   PID  PPID  PGID   SESS TTY      STAT COMM
  501  5219 49433  5219      0 ttys001  T    ./Debug/procgrp/shelljob
  501  5220  5219  5219      0 ttys001  T    ./Debug/procgrp/shelljob


----------------------------------------------------------------------
Test 3: Suspend / Continue / bg / fg / Send signal to a job (process group)
----------------------------------------------------------------------

> ./Debug/procgrp/shelljob
I'm a parent (PID:5219), I have a child (PID:5220)

^Z
[2]  + 5219 suspended
> jobs
[1]  - suspended  vim
[2]  + suspended  ./Debug/procgrp/shelljob
> myps -p 5219,5220
  UID   PID  PPID  PGID   SESS TTY      STAT COMM
  501  5219 49433  5219      0 ttys001  T    ./Debug/procgrp/shelljob
  501  5220  5219  5219      0 ttys001  T    ./Debug/procgrp/shelljob
> bg %2
[2]  - 5219 continued  ./Debug/procgrp/shelljob
> jobs
[1]  + suspended  vim
[2]  - running    ./Debug/procgrp/shelljob
> myps -p 5219,5220
  UID   PID  PPID  PGID   SESS TTY      STAT COMM
  501  5219 49433  5219      0 ttys001  S    ./Debug/procgrp/shelljob
  501  5220  5219  5219      0 ttys001  S    ./Debug/procgrp/shelljob
> fg %2
[2]  - 5219 running    ./Debug/procgrp/shelljob
^Z
[2]  + 5219 suspended  ./Debug/procgrp/shelljob
> jobs
[1]  - suspended  vim
[2]  + suspended  ./Debug/procgrp/shelljob

> kill -CONT %2
> jobs
[1]  - suspended  vim
[2]  + running    ./Debug/procgrp/shelljob

> kill -TSTP %2
[2]  + 5219 suspended  ./Debug/procgrp/shelljob
> jobs
[1]    suspended  vim
[2]  + suspended  ./Debug/procgrp/shelljob

> kill -19 -5219
> jobs
[1]    suspended  vim
[2]  + running    ./Debug/procgrp/shelljob

> kill -18 -5219
> jobs
[1]    suspended  vim
[2]  + suspended  ./Debug/procgrp/shelljob


----------------------------------------------------------------------
Test 4: Run a background job
----------------------------------------------------------------------
> ./Debug/procgrp/shelljob &
[2] 66575
> I'm a parent (PID:66575), I have a child (PID:66578)
>
> jobs
[1]  + suspended  vim
[2]    running    ./Debug/procgrp/shelljob
> myps -p 66575,66578
  UID   PID  PPID  PGID   SESS TTY      STAT COMM
  501 66575 49433 66575      0 ttys001  SN   ./Debug/procgrp/shelljob
  501 66578 66575 66575      0 ttys001  SN   ./Debug/procgrp/shelljob
> kill %2
[2]    66575 terminated  ./Debug/procgrp/shelljob
> myps -p 66575,66578
  UID   PID  PPID  PGID   SESS TTY      STAT COMM

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
