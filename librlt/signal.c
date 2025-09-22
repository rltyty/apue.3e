#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "rltapue.h"

void pr_statsig(int status) {
  if (WIFSTOPPED(status)) {
    printf("Child stopped by signal %d (%s)\n", WSTOPSIG(status),
           strsignal(WSTOPSIG(status)));
  } else if (WIFEXITED(status)) {
    printf("Child exited with status %d\n", WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    printf("Child killed by signal %d (%s)\n", WTERMSIG(status),
           strsignal(WTERMSIG(status)));
  } else if (WIFCONTINUED(status)) {
    printf("Child continued by signal %d (%s)\n", SIGCONT, strsignal(SIGCONT));
  } else {
    printf("Unknown status %d \n", status);
  }
}

char *pidstr(pid_t pid) {
  char *s = malloc(8);
  if (!s) return NULL;
  snprintf(s, 8, "%d", pid);
  return s;
}

void ps(pid_t pid) {
  char fmt[100] = "ps -o uid,pid,ppid,pgid,sess,tty,state,comm -p %d";
  char *cmd = malloc(128);
  if (sprintf(cmd, fmt, pid) < 0) {
    my_perror("error: sprintf");
  }
  system(cmd);
}

void pr_mask(const char *str) {
  sigset_t set;
  int errno_save = errno; /* we can be called by signal handlers */
  if (sigprocmask(0, NULL, &set) < 0) {
    err_ret("sigprocmask error");
  } else {
    pr_mask3(str, set);
  }
  errno = errno_save; /* restore errno */
}

void pr_mask2(const char *str, sigset_t *mask) {
  printf("%s", str);
  if (sigismember(mask, SIGINT)) printf(" SIGINT");
  if (sigismember(mask, SIGQUIT)) printf(" SIGQUIT");
  if (sigismember(mask, SIGUSR1)) printf(" SIGUSR1");
  if (sigismember(mask, SIGUSR2)) printf(" SIGUSR2");
  if (sigismember(mask, SIGALRM)) printf(" SIGALRM");
  if (sigismember(mask, SIGABRT)) printf(" SIGABRT");
  if (sigismember(mask, SIGTSTP)) printf(" SIGTSTP");
  if (sigismember(mask, SIGCHLD)) printf(" SIGCHLD");

  // this two are never blocked
  if (sigismember(mask, SIGKILL)) printf(" SIGKILL");
  if (sigismember(mask, SIGSTOP)) printf(" SIGSTOP");

  /* remaining signals can go here  */

  printf("\n");
}

void pr_mask3(const char *str, sigset_t mask) {
  printf("%s: ", str);
  while (mask > 0) {
    sigset_t lsb = -mask & mask; // find least significant bit (the rightmost 1)
    int signo = __builtin_ctzl(lsb) + 1; // use GCC/Clang count trailing zeros
    printf("|%s|, ", strsignal(signo));
    sigdelset(&mask, signo);
  }
  printf("\n");
}


#ifdef test_signal
#undef test_signal
int main(void) { 
  sigset_t mask, old;
  sigemptyset(&mask);

  int signals[] = {SIGUSR1, SIGUSR2, SIGABRT, SIGCONT, SIGTSTP, SIGSTOP};
  for (int i = 0; i < sizeof(signals) / sizeof(int); i++) {
    sigaddset(&mask, signals[i]);
  }
  sigemptyset(&old);
  sigprocmask(SIG_SETMASK, &mask, &old);
  pr_mask3("Current blocked signals", mask);
  sigprocmask(SIG_SETMASK, &old, NULL);

  pr_sep('-', 60);

  sigfillset(&mask);
  sigprocmask(SIG_SETMASK, &mask, &old);
  pr_mask("Current blocked signals");
  sigprocmask(SIG_SETMASK, &old, NULL);
  return 0;
}
#endif
