#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_pid_t.h>
#include <unistd.h>
#include "rltapue.h"

volatile sig_atomic_t got_alarm = 0;

void sig_alrm(int);

int main(int argc, char *argv[]) {
  pid_t pid;
  if ((pid = fork()) < 0) {
    my_perror("error: fork");
  } else if (pid == 0) { // child
    sleep(2);
    printf("I'm child: %d, my parent ps info:\n", getpid());
    ps(getppid());
    exit(0);
  } else {
    if (signal(SIGALRM, sig_alrm) == SIG_ERR) {
      my_perror("error: signal");
    }
    printf("I'm parent: %d\n", getpid());
    alarm(3);
    while (got_alarm == 0) {
      pause();    // returns after any caught signal is delivered
                  // during pause, the process is still in running/sleep state
                  // not suspended
    }
    printf("Alarm: Time out!\n");
  }
  return 0;
}

void sig_alrm(int signo) {
  got_alarm = 1;
  printf("sig_alrm handler: I'm: %d\n", getpid());
}

/*

> ./Debug/signals/sigalarm2
I'm parent: 74574
I'm child: 74575, my parent ps info:
  UID   PID  PPID  PGID   SESS TTY      STAT COMM
  501 74574 49433 74574      0 ttys001  S+   ./Debug/signals/sigalarm2
sig_alrm handler: I'm: 74574
Alarm: Time out!

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
