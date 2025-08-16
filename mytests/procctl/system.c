#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

/* No signal handling version */

int system(const char *cmdstring) {
  pid_t pid;
  int status;
  /* version without signal handling */
  if (cmdstring == NULL) return (1); /* always a command processor with UNIX */
  if ((pid = fork()) < 0) {
    status = -1;         /* probably out of processes */
  } else if (pid == 0) { /* child */
    execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
    _exit(127); /* execl error */
  } else {      /* parent */
    while (waitpid(pid, &status, 0) < 0) { // <- waitpid: option=0, blocking
                                           // mode wait until the pid process
                                           // terminates. while(.. < 0) is for
                                           // signal interrupt, waitpid()
                                           // returns -1
      if (errno != EINTR) {
        status = -1; /* error other than EINTR from waitpid() */
        break;
      }
    }
  }
  return (status);
}

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    printf("Run command:[%s]\n", argv[i]);
    system(argv[i]);
  }
}

/*
> ./Debug/procctl/system date ls cal
Run command:[date]
Fri Aug 15 19:15:59 CST 2025
Run command:[ls]
Debug     compile_commands.json  filedir  general      procctl  standards  tmp
Makefile  data                   fileio   mytests.inc  procenv  stdio
Run command:[cal]
    August 2025
Su Mo Tu We Th Fr Sa
                1  2
 3  4  5  6  7  8  9
10 11 12 13 14 15 16
17 18 19 20 21 22 23
24 25 26 27 28 29 30
31
 */
