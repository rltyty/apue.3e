#include "rltapue.h"
#include <errno.h>
#include <limits.h>
#include <sys/resource.h>
#include <unistd.h>

#include <stdio.h>

// #ifdef OPEN_MAX
// static long openmax = OPEN_MAX;
// #else
// static long openmax = 0;
// #endif

#define OPEN_MAX_GUESS 256

long open_max(void) {
  int openmax = 0;
  struct rlimit rl;
  if (getrlimit(RLIMIT_NOFILE, &rl) != 0) { // ulimit -n
    my_perror("grlimit failed");
  } else {
    // printf("Soft limit (current): %lu\n", (unsigned long)rl.rlim_cur);
    // printf("Hard limit (maximum): %lu\n", (unsigned long)rl.rlim_max);
  }
  errno = 0;
  if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
    my_perror("sysconf: _SC_OPEN_MAX is not supported");
    openmax = MAX(rl.rlim_cur, OPEN_MAX_GUESS);
  } else {
    openmax = MIN(rl.rlim_cur, openmax);
  }
  return openmax;
}

#ifdef test_openmax
#include <stdio.h>
int main(void) { printf("open_max()=%ld\n", open_max()); }
#endif
