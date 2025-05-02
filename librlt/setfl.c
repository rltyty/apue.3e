#include <fcntl.h>
#include <stdio.h>

void set_fl(int fd, int flags) { /* flags are file status flags to turn on */

  int val;

  if ((val = fcntl(fd, F_GETFL, 0)) < 0)
    perror("fcntl F_GETFL error");

  val |= flags; /* turn on flags */

  if (fcntl(fd, F_SETFL, val) < 0)
    perror("fcntl F_SETFL error");
}


#ifdef test_setfl
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include "rltapue.h"

int main(void) {
  char *path = "tmp/data/f1.txt";
  int fd = -1;

  if ((fd = open(path, O_RDWR)) == -1) {
    my_perror("open file [%s] failed.", path);
  }

  int val;
  if ((val = fcntl(fd, F_GETFL, 0)) < 0)
    perror("fcntl F_GETFL error");

#ifdef __APPLE__
  assert(val == O_RDWR);
  assert(O_RDWR == 02 && O_RDWR == 0x0002);
  printf("On macOS, fsf = 0x%x, no extra flag set beyond oflag, "
         "like __O_LARGEFILE\n", val);
#elif defined(__linux__)
#ifdef __x86_64__
  assert(val == (O_RDWR | 0100000) && val == (O_RDWR | 0x8000));
  printf("On Linux x86_64, fsf = 0x%x, with extra bit 0x8000\n", val);
#elif defined(__aarch64__)
  assert(val == (O_RDWR | 0400000) && val == (O_RDWR | 0x20000));
  printf("On Linux aarch64, fsf = 0x%x, with extra bit 0x20000\n", val);
#endif
#endif

  set_fl(fd, O_APPEND);      // macOS: 0x00000008, Debian: 0x0400(02000)
  if ((val = fcntl(fd, F_GETFL, 0)) < 0)
    perror("fcntl F_GETFL error");

#ifdef __APPLE__
  assert(val == (O_RDWR | O_APPEND));
#elif defined(__linux__)
  printf("On Linux, only the O_APPEND, O_ASYNC, O_DIRECT, O_NOATIME, "
         "and O_NONBLOCK flags can be changed by F_SETFL operation.\n");
#ifdef __x86_64__
  assert(val == (O_RDWR | O_APPEND | 0x8000) && val == 0x8402);
#elif defined(__aarch64__)
  assert(val == (O_RDWR | O_APPEND | 0x20000) && val == 0x20402);
#endif
#endif

// NOTE:
// fcntl() provides for control over descriptors.
// 1. Headers:
// <fcntl.h>
//   <feature.h> -- set feature as per CFLAGS: -DLINUX -D_GNU_SOURCE
//   <bits/fcntl.h>
//     <bits/fcntl-linux.h> (/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h)
//     The various #ifndef cases allow the architecture specific file to
//     define those values with different values.
// 2. On Linux, this operation can change only the O_APPEND, O_ASYNC,
// O_DIRECT, O_NOATIME, and O_NONBLOCK flags.  It is not possible to change
// the O_DSYNC and O_SYNC flags; So, It is not possible to use F_SETFL to
// change the state of the O_DSYNC and O_SYNC flags. Attempts to change the
// state of these flags are silently ignored. (See fcntl(2) man)

  set_fl(fd, O_SYNC);      // macOS: 0x0080, Debian: 0x101000(04010000)

#ifdef __APPLE__
  if ((val = fcntl(fd, F_GETFL, 0)) < 0)
    perror("fcntl F_GETFL error");
  assert(val == (O_RDWR | O_APPEND | O_SYNC)); // macOS: 0x8a (0x2|0x8|0x80)
#elif defined(__linux__)
  int ignored;
  if ((ignored = fcntl(fd, F_GETFL, 0)) < 0)
    perror("fcntl F_GETFL error");
  assert(val == ignored);
#endif

}

#endif
