#include <signal.h>
#include <sys/signal.h>
#include "rltapue.h"

void sig_intr(int);
void sig_usr1(int);

static volatile int intr_flag = 0;

int main(int argc, char *argv[]) {

  printf ("Let's examine how the signal mask of the process is changed:\n\n");

  struct sigaction act, oact;
  sigemptyset(&act.sa_mask); /* Initialize sa_mask to empty */
  act.sa_flags = 0;           /* Set flags to 0 for default behavior */
  act.sa_handler = sig_usr1;
  if (sigaction(SIGUSR1, &act, &oact) < 0) {
    my_perror("error: sigaction");
  } else {
    printf("Original signal handler for SIGUSR1: %p\n", oact.sa_handler);
  }

  Sigfunc *pOHand;
  if ((pOHand = signal(SIGINT, sig_intr)) == SIG_ERR) {
    my_perror("error: signal");
  } else {
    printf("Original signal handler for SIGINT: %p\n", pOHand);
  }

  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGABRT);
  sigaddset(&mask, SIGQUIT);
  sigaddset(&mask, SIGINT);
  sigprocmask(SIG_SETMASK, &mask, NULL);
  pr_mask("Main: initial setup signal mask after sigprocmask: ");

  while(1) {
    pause();
    if (intr_flag == 1) {
      sigemptyset(&mask);
      sigaddset(&mask, SIGINT);
      sigprocmask(SIG_UNBLOCK, &mask, NULL);
      pr_mask("Main: after sigprocmask, unblock SIGINT: ");
      intr_flag = 2;
    }
    pr_mask("Main: ");
  }

  return 0;
}

void sig_intr(int signo) {
  pr_mask("sig_intr: ");
  if (intr_flag == 3) {
    printf("exit(1111)\n");
    exit(1111); // normal exit: $? = status % 256 = 87
  }
}

void sig_usr1(int signo) {
  pr_mask("sig_usr1: ");
  if (intr_flag == 0) {
    printf("sig_usr1: change intr_flag to 1\n");
    intr_flag = 1;
  } else if (intr_flag == 2) {
    printf("sig_usr1: change intr_flag to 3\n");
    intr_flag = 3;
  } else {
    printf("sig_usr1: intr_flag is %d\n", intr_flag);
  }
}


/*

# Shell A:
> ./Debug/signals/sigmask2
Let's examine how the signal mask of the process is changed:

Original signal handler for SIGUSR1: 0x0
Original signal handler for SIGINT: 0x0
Main: initial setup signal mask after sigprocmask:  SIGINT SIGQUIT SIGABRT
^C^C^Csig_usr1:  SIGINT SIGQUIT SIGUSR1 SIGABRT   <-- kill -USR1 81753
sig_usr1: change intr_flag to 1
sig_intr:  SIGINT SIGQUIT SIGABRT                 <-- unblocked pending signal delivered before sigprocmask returns
Main: after sigprocmask, unblock SIGINT:  SIGQUIT SIGABRT
Main:  SIGQUIT SIGABRT
^Csig_intr:  SIGINT SIGQUIT SIGABRT               <-- avoid re-entrancy issue during the time of execution a handler
Main:  SIGQUIT SIGABRT
sig_usr1:  SIGQUIT SIGUSR1 SIGABRT                <-- kill -USR1 81753
sig_usr1: change intr_flag to 3
Main:  SIGQUIT SIGABRT
^Csig_intr:  SIGINT SIGQUIT SIGABRT
exit(1111)
> echo $?
87                                                <-- normal exit, status % 256

# Shell B:
> pgrep sigmask
81753
> kill -USR1 81753
> kill -USR1 81753

NOTE:
1. When a signal (e.g. SIGINT) is blocked, only one instance is recorded
   as pending after sending multiple <CTRL-C>s.
2. At least one of unblocked pending signals is selected to deliver before
   sigprocmask(2), which change the mask setting, returns.
3. During the execution of signal-catching handler, the signal causes it is
   temporarily added to the mask and removed after the execution, to avoid
   re-entrancy issue (recursive calls of the handler).
4. Two types of terminations and different shell return code:
   4.1 Normal `exit(n)`: rc = n % 256
   4.2 Terminated by signal: rc = 128 + <signal number>
 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
