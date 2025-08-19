#include <stdio.h>
#include <string.h>
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
    printf("Child continued by signal %d (%s)\n", SIGCONT,
           strsignal(SIGCONT));
  }
}
