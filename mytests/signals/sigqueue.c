#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rltapue.h"

#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 199309L && \
    !defined(__APPLE__)
#define HAVE_SIGQUEUE 1
#else
#define HAVE_SIGQUEUE 0
#endif

void sig_hand_sa(int signo, siginfo_t *info, void *context) {
  printf("sig_hand: Signal [%d] was caught by Process[%d]\n", signo, getpid());
  switch (info->si_code) {
    case SI_QUEUE:
      printf("sent via sigqueue, sival_int=%d\n", info->si_value.sival_int);
      break;
    case SI_USER:
      printf("sent via kill or raise\n");
      break;
    default:
      printf("si_code=%d\n", info->si_code);
  }
}

int main(void) {
  struct sigaction act = {0};
  act.sa_flags |= SA_SIGINFO;
  act.sa_sigaction = sig_hand_sa;
  int signo;
#if HAVE_SIGQUEUE
  signo = SIGRTMIN;
#else
  signo = SIGUSR1;
#endif
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, signo);

  pid_t pid;
  if ((pid = fork()) < 0) {
    my_perror("error: fork");
  } else if (pid == 0) {  // child
    sigprocmask(SIG_BLOCK, &mask, NULL);
    sigaction(signo, &act, NULL);
    sleep(6);
    printf("Child[%d]: is awake. Unblock signal mask.\n", getpid());
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    printf("Child[%d]: Exiting..\n", getpid());
    exit(0);
  } else {  // parent
    for (int i = 0; i < 3; i++) {
      sleep(1);
#if HAVE_SIGQUEUE
      printf("Parent: siggueue: send [%s]:[%d] to child[%d].\n",
             strsignal(signo), signo, pid);
      union sigval sv;
      sv.sival_int = i;
      sigqueue(pid, signo, sv);
#else
      printf("Parent: kill: send [%s]:[%d] to child[%d].\n", strsignal(signo),
             signo, pid);
      kill(pid, signo);
#endif
    }
    wait(NULL);
    puts("Parent: Exiting..");
  }
}
// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
