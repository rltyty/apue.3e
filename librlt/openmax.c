#include "rltapue.h"
#include <errno.h>
#include <limits.h>
#include <sys/resource.h>
#include <unistd.h>

#include <stdio.h>
/*
 * The maximum number of open files is one of the limits on the consumption
 * of system resources by the current process and each process it creates.

 * Two ways to obtain the value:
 * - getrlimit(RLIMIT_NOFILE) (soft/hard limits)
 * - sysconf(_SC_OPEN_MAX) (runtime query)
 *
 * macOS:
 * - OPEN_MAX (from <sys/syslimits.h>, derived from 4.4 BSD), compile-time
 * Linux:
 * - a fixed OPEN_MAX is deprecated;
 * - sysconf() (sysdeps/unix/sysv/linux/sysconf.c) ->
 *     posix_sysconf() (sysdeps/posix/sysconf.c) ->
 *       __getdtablesize() (sysdeps/posix/getdtsz.c)
 *         ```c
 *         __getrlimit (RLIMIT_NOFILE, &ru) < 0 ? OPEN_MAX : ru.rlim_cur;
 *         ```
 *   So, sysconf calls getrlimit, return the soft limit.
 */

long open_max(void) {
  long openmax;
  if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
    my_perror("sysconf: _SC_OPEN_MAX is not supported");
  } else if (openmax == LONG_MAX) {
    openmax = OPEN_MAX_GUESS;
  }
  return openmax;
}

#ifdef test_openmax
#undef test_openmax
#include <stdio.h>
int main(void) {
  printf("open_max()=%ld\n", open_max());
  struct rlimit rl;
  if (getrlimit(RLIMIT_NOFILE, &rl) != 0) { // ulimit -n
    my_perror("grlimit failed");
  }
  printf("Soft limit (current): %lu\n", (unsigned long)rl.rlim_cur);
  printf("Hard limit (maximum): %lu\n", (unsigned long)rl.rlim_max);
}
#endif
