#include <signal.h>
#include <sys/signal.h>

int main(int argc, char *argv[]) {
  sigset_t set;
  sigaddset(&set, SIGQUIT);
  return 0;
}

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
