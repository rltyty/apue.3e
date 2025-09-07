#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include "time.h"

static void sig_alrm(int signo) {
  /* nothing to do, just returning wakes up sigsuspend() */

  /* Just for mark the difference for test */
  printf("sig_alrm\n");
}


unsigned int sleep(unsigned int seconds) {
  struct sigaction newact, oldact;
  sigset_t newmask, oldmask, suspmask;
  unsigned int unslept;

  /* set our handler, save previous information */
  newact.sa_handler = sig_alrm;
  sigemptyset(&newact.sa_mask);
  newact.sa_flags = 0;
  sigaction(SIGALRM, &newact, &oldact);

  /* block SIGALRM and save current signal mask */
  sigemptyset(&newmask);
  sigaddset(&newmask, SIGALRM);
  sigprocmask(SIG_BLOCK, &newmask, &oldmask);

  alarm(seconds);
  suspmask = oldmask;

  /* make sure SIGALRM isn't blocked */
  sigdelset(&suspmask, SIGALRM);

  /* wait for any signal to be caught */
  sigsuspend(&suspmask);

  /* some signal has been caught, SIGALRM is now blocked */

  unslept = alarm(0);

  /* reset previous action */
  sigaction(SIGALRM, &oldact, NULL);

  /* reset signal mask, which unblocks SIGALRM */
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
  return (unslept);
}

int main(int argc, char *argv[]) {
  int timeout = 5;
  if (argc > 1) {
    int n = atoi(argv[1]);
    if (n > 0) timeout = n;
  }
  printf("Call local sleep(%d)\n", timeout);
  sleep(timeout);   // strong definition overrides weak alias of libc

  // unsigned int (*libc_sleep)(unsigned int) = NULL;
  __typeof(sleep) *libc_sleep = dlsym(RTLD_NEXT, "sleep");
  printf("Call the C standard library sleep(%d)\n", timeout);
  libc_sleep(timeout);
  nanosleep(NULL, NULL);
  return 0;
}

/*
> Debug/signals/sleep
Call local sleep(5)
sig_alrm
Call the C standard library sleep(5)
>
 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
