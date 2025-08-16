#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include "rltapue.h"

const char * const PS_CMD = "ps -o pid,ppid,state,command -p %d";
const char * const ZOMBIE_PID_FILE = "./tmp/data/procctl/zombie.pid";

int main(int argc, char *argv[]) {
  pid_t pid;

  if ((pid = fork()) < 0) {
    my_perror("error: fork()");
  } else if (pid == 0) {  // child
    if ((pid = fork()) < 0) {
      my_perror("error: fork()");
    } else if (pid == 0) {  // grandchild
      sleep(5);
    } else {  // child
      int fd = open(ZOMBIE_PID_FILE, O_WRONLY|O_CREAT|O_TRUNC, FILE_MODE);
      if (fd < 0) {
      my_perror("error: child PID[%d]: open()", getpid());
      } else {
        char buf[10] = {0};
        sprintf(buf, "%d", pid);
        if (write(fd, buf, strlen(buf)) < 0) { // write grandchild pid to file
          my_perror("error: write()");
        };
      }
      _exit(0);
    }
  } else {  // parent
    sleep(3);
    printf("No zombie, but orphan adopted by init.\n");
    int fd = open(ZOMBIE_PID_FILE, O_RDONLY);
    if (fd < 0) {
      my_perror("error: parent PID[%d]: open()", getpid());
    } else {
      char buf[10] = {0};
      int n;
      if ((n = read(fd, buf, sizeof(buf))) < 0) {
        my_perror("error: read");
      } else {
        pid = atoi(buf);
        printf("grandchild: PID[%d]\n", pid);
        char *cmd = (char *)malloc(25);
        sprintf(cmd, PS_CMD, pid);
        system(cmd);
        free(cmd);
      }
    }
  }

  return 0;
}

/*
Sample output:

> ./Debug/procctl/zombie2
No zombie, but orphan adopted by init.
grandchild: PID[60433]
  PID  PPID STAT COMMAND
60433     1 S+   ./Debug/procctl/zombie2
*/
