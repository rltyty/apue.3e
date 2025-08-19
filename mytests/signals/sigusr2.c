#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rltapue.h"


static void sig_usr(int);

int main(int argc, char *argv[]) {
  pid_t pid;

  if (signal(SIGUSR1, sig_usr) == SIG_ERR) my_perror("can't catch SIGUSR1");
  if (signal(SIGUSR2, sig_usr) == SIG_ERR) my_perror("can't catch SIGUSR2");

  if ((pid = fork()) < 0) {
    my_perror("error: fork");
  } else if (pid == 0) {  // child
    while (1) pause();
    _exit(1);
  } else {  // parent
    sleep(3);
    printf("I'm a parent (PID:%d), I have a child (PID:%d)\n\n", getpid(), pid);
    int rc;
    while(waitpid(pid, &rc, WUNTRACED) > 0) {
      pr_statsig(rc);
    };
  }

  return 0;
}

static void sig_usr(int signo) /* argument is signal number */
{
  if (signo == SIGUSR1)
    printf("received SIGUSR1\n");
  else if (signo == SIGUSR2)
    printf("received SIGUSR2\n");
  else
    my_perr_dump("received signal %d\n", signo);
}

/*

> ./Debug/signals/sigusr2 &
[2] 50005
> I'm a parent (PID:50005), I have a child (PID:50007)
>
> kill -USR1 50005
received SIGUSR1

> kill -USR2 50007
received SIGUSR2

> kill -2 50007
Child killed by signal 2 (Interrupt: 2)
[2]    50005 done       ./Debug/signals/sigusr2

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
