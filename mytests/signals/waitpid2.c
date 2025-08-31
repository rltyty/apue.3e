#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// Signal handler for SIGCHLD
void sigchld_handler(int signo) {
  int status;
  pid_t pid;

  printf("Handler: Enter >>> \n");
  // Use a loop to reap all terminated children,
  // as multiple children might exit before the handler is called
  // waitpid with WNOHANG ensures the handler doesn't block
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    if (WIFEXITED(status)) {
      printf("Handler: Child %d exited normally with status %d\n", pid,
             WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      printf("Handler: Child %d terminated by signal %d\n", pid,
             WTERMSIG(status));
    }
  }

  if (pid == -1 && errno != ECHILD) {
    perror("waitpid in handler failed");
  }

  printf("Handler: Exit <<< \n");
}

int main() {
  struct sigaction sa;

  // Configure sigaction for SIGCHLD
  sa.sa_handler = sigchld_handler;  // Set the handler function
  sigemptyset(&sa.sa_mask);  // Clear the mask, no signals blocked in handler
  sa.sa_flags =
      SA_RESTART | SA_NOCLDSTOP;  // SA_RESTART for system calls, SA_NOCLDSTOP
                                  // to not get signals for stopped children

  // Install the signal handler
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction failed");
    exit(EXIT_FAILURE);
  }

  printf("Parent PID: %d\n", getpid());

  // Fork multiple child processes
  for (int i = 0; i < 10; i++) {
    pid_t pid = fork();

    if (pid == -1) {
      perror("fork failed");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      // Child process
      printf("Child %d (PID: %d) starting, will sleep for %d seconds...\n",
             i + 1, getpid(), i + 1);
      sleep(i + 1);  // Children sleep for different durations
      printf("Child %d (PID: %d) exiting.\n", i + 1, getpid());
      exit(100 + i);  // Exit with a unique status
    }
  }

  // Parent's main loop (doing other work)
  printf("Parent doing other work...\n");
  for (int i = 0; i < 10; i++) {
    printf("Parent: Working... (%d/10)\n", i + 1);
    sleep(1);
  }

  printf("Parent: All children should have been reaped by now.\n");
  printf("Parent: Exiting.\n");

  return 0;
}

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
