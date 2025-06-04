#include "rltapue.h"
#include <fcntl.h>
#include <stdio.h>

void test_features() {
#ifdef LINUX
  printf("On Linux, <fcntl.h> includes <features.h>, in which compilation "
         "environment is determined and macros are defined.\n");
#ifdef _GNU_SOURCE
  printf("When compiled with _GNU_SOURCE, \n");
#endif
#ifdef _LARGEFILE64_SOURCE
  printf("_LARGEFILE64_SOURCE was defined to be %d.\n", _LARGEFILE64_SOURCE);
#endif
#ifdef __USE_LARGEFILE64
  printf("__USE_LARGEFILE64 was defined to be %d further.\n",
         __USE_LARGEFILE64);
#endif
#ifdef O_LARGEFILE
  printf("O_LARGEFILE = %d.\n", O_LARGEFILE);
#endif
#ifdef __USE_FILE_OFFSET64
  printf("__USE_FILE_OFFSET64 = %d.\n", __USE_FILE_OFFSET64); // not defined
#endif
#ifdef __O_LARGEFILE
  printf("__O_LARGEFILE defined.\n");
#endif

  // NOTE:
  // open64.c (glibc:
  // /usr/src/glibc/glibc-2.36/sysdeps/unix/sysv/linux/open64.c)
  //  __libc_open64
  //    return SYSCALL_CANCEL (openat, fd, file, oflag | O_LARGEFILE, mode);

#elif defined(MACOS)
  printf("On macOS, <fcntl.h> use <sys/cdefs.h> for feature determnination");
#ifdef _DARWIN_C_SOURCE
  printf("When compiled with _DARWIN_C_SOURCE, \n");
#endif
#endif
}

int main(void) {
  // 1.
  long m = open_max();
  printf("open_max() returns %ld\n", m);

  // 2.
  test_features();
}
