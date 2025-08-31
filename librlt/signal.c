#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_pid_t.h>
#include <sys/signal.h>
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
    pr_mask2(str, &set);
  }
  errno = errno_save; /* restore errno */
}

void pr_mask2(const char *str, sigset_t *set) {
  printf("%s", str);
  if (sigismember(set, SIGINT)) printf(" SIGINT");
  if (sigismember(set, SIGQUIT)) printf(" SIGQUIT");
  if (sigismember(set, SIGUSR1)) printf(" SIGUSR1");
  if (sigismember(set, SIGUSR2)) printf(" SIGUSR2");
  if (sigismember(set, SIGALRM)) printf(" SIGALRM");
  if (sigismember(set, SIGABRT)) printf(" SIGABRT");
  if (sigismember(set, SIGTSTP)) printf(" SIGTSTP");
  if (sigismember(set, SIGCHLD)) printf(" SIGCHLD");

  // this two are never blocked
  if (sigismember(set, SIGKILL)) printf(" SIGKILL");
  if (sigismember(set, SIGSTOP)) printf(" SIGSTOP");

  /* remaining signals can go here  */

  printf("\n");
}
