#include "rltapue.h"
#include <errno.h>
#include <limits.h>
#include <unistd.h>

#ifdef OPEN_MAX
  static long openmax = OPEN_MAX;
#else
  static long openmax = 0;
#endif

#define OPEN_MAX_GUESS 256

long open_max(void) {
  if (openmax == 0) {
    errno = 0;
    if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
        my_perror("sysconf: _SC_OPEN_MAX is not supported");
    } else {
      openmax = OPEN_MAX_GUESS;
    }
  }
  return openmax;
}

#ifdef test_openmax
#include <stdio.h>
int main(void) { printf("open_max()=%ld\n", open_max()); }
#endif
