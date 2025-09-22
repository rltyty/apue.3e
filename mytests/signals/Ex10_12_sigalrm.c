#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "rltapue.h"

static char _1GB[1 << 30] = {
  [65] = 'A',
  [97] = 'a',
};

void sig_hand(int signo) {
  fprintf(stderr, "Signal[%d] was caught, [%s]\n", signo, strsignal(signo));
}

void sigalrm_test()
{
  struct sigaction act = {0};
  act.sa_handler = sig_hand;
  if (sigaction(SIGALRM, &act, NULL) < 0) {
    my_perror("error: sigaction");
  }
  alarm(1);
  size_t n = fwrite(_1GB, sizeof(char), 1 << 30, stdout);
  if (n < (1 << 30)) {
    my_perror_ret("fwrite: n = %ld", n);;
  }
}

int main(int argc, char *argv[]) {
  sigalrm_test();
  return 0;
}

/**

On macOS:

SIGALRM was caught during fwrite, after returned from the handler, fwrite continued to finish the writing.

> ./Debug/signals/Ex10_12_sigalrm > a
Signal[14] was caught, [Alarm clock: 14]

> ll -h a
-rw-r--r-- 1 gpanda staff 1.0G Sep 19 22:11 a

On Linux:

SIGALRM seems to be blocked or ignored during fwrite.

> ./Debug/signals/Ex10_12_sigalrm > a

> ll -h a
-rw-r--r-- 1 rltyty rltyty 1.0G Sep 21 07:52 a

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
