#include <signal.h>
#include <sys/signal.h>
#include "rltapue.h"

int main(int argc, char *argv[]) {
  sigset_t mask, old, delta;
  sigemptyset(&old);
  sigprocmask(0, NULL, &old);             // get current masked signals
  pr_mask2("      Old Mask: ", &old);
  pr_mask ("  Current Mask: ");

  sigemptyset(&delta);
  sigaddset(&delta, SIGABRT);
  pr_mask2(" Block signals: ", &delta);
  sigprocmask(SIG_BLOCK, &delta, NULL);   // union of current and delta
  pr_mask ("  Current Mask: ");

  sigfillset(&mask);
  sigdelset(&mask, SIGABRT);              // remove SIGABRT from complete set
  pr_mask2("   Mask to set: ", &mask);
  sigprocmask(SIG_SETMASK, &mask, &old);  // set current to mask (exclude
                                          // SIGKILL & SIGSTOP)
  pr_mask ("  Current Mask: ");

  sigemptyset(&delta);
  sigaddset(&delta, SIGQUIT);
  pr_mask2("Unblock signal: ", &delta);
  sigprocmask(SIG_UNBLOCK, &delta, &old); // subtract delta from current
  pr_mask ("  Current Mask: ");

  sigemptyset(&delta);
  sigaddset(&delta, SIGSTOP);
  sigaddset(&delta, SIGKILL);
  pr_mask2(" Block signals: ", &delta);
  sigprocmask(SIG_BLOCK, &delta, &old);   // union of current and delta
                                          // (exclude SIGKILL & SIGSTOP)
  pr_mask ("  Current Mask: ");

  return 0;
}

/*

> Debug/signals/sigmask
      Old Mask:
  Current Mask:
 Block signals:  SIGABRT
  Current Mask:  SIGABRT
   Mask to set:  SIGINT SIGQUIT SIGUSR1 SIGUSR2 SIGALRM SIGTSTP SIGCHLD SIGKILL SIGSTOP
  Current Mask:  SIGINT SIGQUIT SIGUSR1 SIGUSR2 SIGALRM SIGTSTP SIGCHLD
Unblock signal:  SIGQUIT
  Current Mask:  SIGINT SIGUSR1 SIGUSR2 SIGALRM SIGTSTP SIGCHLD
 Block signals:  SIGKILL SIGSTOP
  Current Mask:  SIGINT SIGUSR1 SIGUSR2 SIGALRM SIGTSTP SIGCHLD

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
