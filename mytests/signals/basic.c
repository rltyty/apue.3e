#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "rltapue.h"

int main(int argc, char *argv[]) {
  int pid;
  if ((pid = fork()) < 0) {
    my_perror("error: fork");
  } else if (pid == 0) { // child
    sleep(10);
    exit(0);
  } else { // parent
    sleep(3);
    int rc = 0;
    kill(pid, SIGSTOP); // send signal 17 to the child
    if (waitpid(pid, &rc, WUNTRACED) == -1) {
      my_perror("error: waitpid");
      exit(1);
    }
    assert(WIFSTOPPED(rc));
    printf("child rc = %d\n", rc);
    pr_statsig(rc);
  }
  return 0;
}

/*
 * Sample output:
 *
!Debug/signals/basic
child rc = 4479
Child stopped by signal 17 (Suspended (signal): 17)
 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
