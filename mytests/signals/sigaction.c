#include <signal.h>
#include <stdio.h>

#include "rltapue.h"

void sig_hand(int signo) {
  // just for test, shouldn't use non-reentrant `printf` in a signal handler
  printf("sig_hand");
}

int main(int argc, char *argv[]) {
  struct sigaction act = {0}, oact = {0};
  act.sa_flags |= SA_RESTART;
  act.sa_handler = sig_hand;

  if (sigaction(SIGINT, &act, &oact) < 0) {
    my_perror("error: sigaction");
  }

  return 0;
}

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
