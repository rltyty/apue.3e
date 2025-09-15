#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rltapue.h"

/**
- Common restartable syscalls (with SA_RESTART):

  - read() (on files, sockets sometimes)
  - write() (on files, sockets sometimes)
  - open() (but not on special files like FIFOs sometimes)
  - close() (usually, but not always meaningful)
  - recv(), recvfrom(), recvmsg()
  - send(), sendto(), sendmsg()
  - wait(), waitpid()
  - msgrcv(), msgsnd() (System V IPC)
  - select(), pselect()
  - poll(), ppoll()
  - readv(), writev()
  - sigprocmask()
  - nanosleep()

- Common non-restartable syscalls (always return EINTR):

  - accept() — on old kernels, sometimes restartable on newer kernels
  - connect() — interrupted during blocking connect
  - sleep() — standard sleep interrupted
  - pause() — always interrupted by signals
  - futex() — some futex operations (depends on flags)
  - recv() / send() on some sockets (especially non-blocking)
  - open() on FIFOs / pipes when no writer is present

 */
void sig_hand(int signo) {
  // just for test, shouldn't use non-reentrant `printf` in a signal handler
  printf("sig_hand: [%s] is caught by PID[%d].\n", strsignal(signo), getpid());
}

void non_restartable_syscall_test() {
  pid_t pid;
  if ((pid = fork()) < 0) {
    my_perror("error: fork");
  } else if (pid == 0) {  // child
    printf("Child[PID=%d]: blocking on pause()...\n", getpid());
    pause();  // not restartable
    printf("Child[PID=%d]: Exiting...\n", getpid());
    exit(0);
  } else {
    sleep(2);
    printf("Parent[PID=%d]: Send [%s] to Child[%d]\n", getpid(),
           strsignal(SIGINT), pid);
    kill(pid, SIGINT);
    wait(NULL);
    printf("Parent[PID=%d]: Exiting non_restartable_syscall_test()\n",
           getpid());
  }
}

void restartable_syscall_test() {
  pid_t pid;
  if ((pid = fork()) < 0) {
    my_perror("error: fork");
  } else if (pid == 0) {  // child
    char buf[BUFSIZ];
    size_t n;
    printf("Child[PID=%d]: blocking on read()...\n", getpid());
    while ((n = read(STDIN_FILENO, buf, sizeof(buf) - 1)) > 0) {
      buf[n] = '\0';
      printf("Child[PID=%d]: Read %ld chars: [%s]\n", getpid(), n, buf);
    }
    if (n == 0) {
      printf("Child[PID=%d]: End of reading.\n", getpid());
      exit(0);
    } else {
      my_perror("Child[PID=%d]: error: read", getpid());
      exit(1);
    }
  } else {
    sleep(1);
    int exitFlag = 3;
    while (exitFlag-- > 0) {
      sleep(3);
      printf("Parent[%d]: Send [%s] to Child[%d]\n", getpid(),
             strsignal(SIGINT), pid);
      kill(pid, SIGINT);
    }
    sleep(1);
    printf(
        "Parent[PID=%d]: Now wait() for the child."
        "Type <CTRL-D> to end the talk.\n\n",
        getpid());
    wait(NULL);
    printf("Parent[PID=%d]: Exiting estartable_syscall_test()\n", getpid());
  }
}

int main(int argc, char *argv[]) {
  struct sigaction act = {0}, oact = {0};
  act.sa_flags |= SA_RESTART;
  act.sa_handler = sig_hand;

  if (sigaction(SIGINT, &act, &oact) < 0) {
    my_perror("error: sigaction");
  }

  pr_sep_msg('-', 25, "non_restartable_syscall_test()");
  non_restartable_syscall_test();
  pr_sep_msg('-', 25, "restartable_syscall_test()");
  restartable_syscall_test();
  return 0;
}

/*
 * Sample:
> ./Debug/signals/sigaction
-------------------------  non_restartable_syscall_test()
------------------------- Child[PID=65200]: blocking on pause()...
Parent[PID=65199]: Send [Interrupt: 2] to Child[65200]
sig_hand: [Interrupt: 2] is caught by PID[65200].
Child[PID=65200]: Exiting...
Parent[PID=65199]: Exiting non_restartable_syscall_test()
-------------------------  restartable_syscall_test()  -------------------------
Child[PID=65245]: blocking on read()...
Parent[65199]: Send [Interrupt: 2] to Child[65245]
sig_hand: [Interrupt: 2] is caught by PID[65245].
Parent[65199]: Send [Interrupt: 2] to Child[65245]
sig_hand: [Interrupt: 2] is caught by PID[65245].
Parent[65199]: Send [Interrupt: 2] to Child[65245]
sig_hand: [Interrupt: 2] is caught by PID[65245].
Parent[PID=65199]: Now wait() for the child.Type <CTRL-D> to end the talk.

Hello, world!
Child[PID=65245]: Read 14 chars: [Hello, world!
]
Hello, world!Child[PID=65245]: Read 13 chars: [Hello, world!]
The difference is: 1) first input ended with new line 2) 2nd input ended with
<CTRL-D> (EOF). Child[PID=65245]: Read 94 chars: [The difference is: 1) first
input ended with new line 2) 2nd input ended with <CTRL-D> (EOF).
]
Child[PID=65245]: End of reading.
Parent[PID=65199]: Exiting estartable_syscall_test()
 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
