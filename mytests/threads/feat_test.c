#include <unistd.h>
#include <stdio.h>

void pthreads_feature_test() {
#ifdef _POSIX_THREADS
  puts("Support POSIX Threads");
#endif

  printf("sysconf(_SC_THREADS)=%ld\n", sysconf(_SC_THREADS));
  printf("sysconf(_SC_VERSION)=%ld\n", sysconf(_SC_VERSION));
  printf("sysconf(_SC_XOPEN_VERSION)=%ld\n", sysconf(_SC_XOPEN_VERSION));
}

int main(int argc, char *argv[]) {

  pthreads_feature_test();
  return 0;
}

/*

On Linux:

:!Debug/threads/feat_test
Support POSIX Threads
sysconf(_SC_THREADS)=200809
sysconf(_SC_VERSION)=200809       <--   POSIX.1-2008 SUSv4
sysconf(_SC_XOPEN_VERSION)=700    <--   Issue 7

On macOS:

:!Debug/threads/feat_test
Support POSIX Threads
sysconf(_SC_THREADS)=200112
sysconf(_SC_VERSION)=200112       <--   POSIX.1-2001 SUSv3
sysconf(_SC_XOPEN_VERSION)=600    <--   Issue 6

By the way, `getconf(1)` command can retrieve standard configuration variables

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
