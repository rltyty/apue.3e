#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include "rltapue.h"

const char * const PS_CMD = "ps -o pid,ppid,state,command -p %d";

int main(int argc, char *argv[]) {
  pid_t pid;

  if ((pid = fork()) < 0) {
    my_perror("error: fork()");
  } else if (pid == 0) {  // child
    printf(
        "I'm a child (PID %d), my parent (PPID %d). I'll become a zombie!\n\n",
        getpid(), getppid());
    sleep(2);
  } else {  // parent
    printf(
        "\nI'm a parent (PID %d). I have a child (PID %d), it is a zombie.\n",
        getpid(), pid);
    sleep(3);
    char *cmd = (char *)malloc(25);
    sprintf(cmd, PS_CMD, pid);  // show the child's state
    system(cmd);
    free(cmd);
  }

  return 0;
}

/*
Sample output:

:!Debug/procctl/zombie
I'm a child (PID 48899), my parent (PPID 48898). I'll become a zombie!

  PID  PPID STAT COMMAND
48899 48898 Z    (zombie)

I'm a parent (PID 48898). I have a child (PID 48899), it is a zombie.

*/
