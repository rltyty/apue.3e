#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rltapue.h"

int main(int argc, char *argv[]) {
  pid_t pid;
  if ((pid = fork()) < 0) {
    my_perror("error: fork()");
  } else if (pid == 0) {  // child - execl
    printf("I'm the CHILD, PID=%d, PARENT PID:%d. Calls exec().\n",
        getpid(), getppid()); // might not seen due to fully buffered when
                              // stdout is connected to Neovim pipe
    if (execl("./Debug/procctl/printargv",
              "printargv", "Leo", "Messi", (char *)0) < 0)
      my_perror("error: execl()");
    printf("I'm the CHILD, never expected to be here "
           "unless exec() fails (to replace process image)\n");
    exit(1);
  } else {  // parent
    int stat = 0;
    wait(&stat);
    printf("I'm the PARENT, PID=%d, CHILD PID:%d\n", getpid(), pid);
    pr_exit(stat);
    printf("I'm the PARENT, Bye!\n");
  }

  pr_sep('-', 70);

  /**
   * Since there is no printargv found in the PATH of current process environ,
   * execlp() fails to open printargv and replace process image. Statements
   * after execlp() call can be reached.
   */
  if ((pid = fork()) < 0) {
    my_perror("error: fork()");
  } else if (pid == 0) { //child - execlp
    printf("I'm the CHILD, PID=%d, PARENT PID:%d. Calls exec().\n",
         getpid(), getppid()); // might not seen due to fully buffered when
                              // stdout is connected to Neovim pipe
   if (execlp("printargv", "printargv", "Hernán", "Crespo", (char*)0) < 0)
      my_perror("error: execlp()");
    printf("I'm the CHILD, never expected to be here "
           "unless exec() fails (to replace process image)\n");
    exit(1);
  } else { // parent
    int stat = 0;
    wait(&stat);
    printf("I'm the PARENT, PID=%d, CHILD PID:%d\n", getpid(), pid);
    pr_exit(stat);
    printf("I'm the PARENT, Bye!\n");
  }


  pr_sep('-', 70);

  /**
   * Add local build target directory to the PATH environment
   */
  add_path("./Debug/procctl");

  if ((pid = fork()) < 0) {
    my_perror("error: fork()");
  } else if (pid == 0) { //child - execle
    printf("I'm the CHILD, PID=%d, PARENT PID:%d. Calls exec().\n",
         getpid(), getppid()); // might not seen due to fully buffered when
                              // stdout is connected to Neovim pipe
   if (execlp("printargv", "printargv", "Hernán", "Crespo", (char*)0) < 0)
      my_perror("error: execlp()");
    printf("I'm the CHILD, never expected to be here "
           "unless exec() fails (to replace process image)\n");
    exit(1);
  } else { // parent
    int stat = 0;
    wait(&stat);
    printf("I'm the PARENT, PID=%d, CHILD PID:%d\n", getpid(), pid);
    pr_exit(stat);
    printf("I'm the PARENT, Bye!\n");
  }

  pr_sep('-', 70);

  /**
   * The envp passed to the call of execle() in the child process is to setup
   * of the environ of new process.
   */

  char *envs[] = {
    "PATH=./Debug/procctl",
    "GOAT=Messi",
    "Forward=Crespo",
    NULL, // (char*) 0
  };

  if ((pid = fork()) < 0) {
    my_perror("error: fork()");
  } else if (pid == 0) { //child - execle
    printf("I'm the CHILD, PID=%d, PARENT PID:%d. Calls exec().\n",
         getpid(), getppid()); // might not seen due to fully buffered when
                              // stdout is connected to Neovim pipe
   if (execle("./Debug/procctl/printenv", "printenv", "PATH", "GOAT", "Forward", NULL, envs) < 0)
      my_perror("error: execle()");
    printf("I'm the CHILD, never expected to be here "
           "unless exec() fails (to replace process image)\n");
    exit(1);
  } else { // parent
    int stat = 0;
    wait(&stat);
    printf("I'm the PARENT, PID=%d, CHILD PID:%d\n", getpid(), pid);
    pr_exit(stat);
    printf("I'm the PARENT, Bye!\n");
  }

  if ((pid = fork()) < 0) {
    my_perror("error: fork()");
  } else if (pid == 0) { //child - execle
    printf("I'm the CHILD, PID=%d, PARENT PID:%d. Calls exec().\n",
         getpid(), getppid()); // might not seen due to fully buffered when
                              // stdout is connected to Neovim pipe
   if (execle("./tmp/data/procctl/a.sh", "./a.sh", "PATH", "GOAT", "Forward", NULL, envs) < 0)
      my_perror("error: execle()");
    printf("I'm the CHILD, never expected to be here "
           "unless exec() fails (to replace process image)\n");
    exit(1);
  } else { // parent
    int stat = 0;
    wait(&stat);
    printf("I'm the PARENT, PID=%d, CHILD PID:%d\n", getpid(), pid);
    pr_exit(stat);
    printf("I'm the PARENT, Bye!\n");
  }


}
