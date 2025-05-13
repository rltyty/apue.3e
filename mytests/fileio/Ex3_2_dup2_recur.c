#include "rltapue.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Recursive version */
/* This solution is limited to file descriptor limit (`ulimit -n`), if
 * the limit is 2048, then valid range is [0, 2047]
 */

int recur(int fd, int newfd) {
  /* error path */
  if (fd == -1)
    return -1;

  /* OK path */
  if (fd == newfd)
    return fd;

  int ret = recur(dup(fd), newfd);
  if (close(fd) == -1)
    return -1;
  return ret;
}

int mydup2(int fd, int newfd) {
  /* test if fd is an active, valid file descriptor */
  /*
  char path[128];
  snprintf(path, 128, "/dev/fd/%d", fd);
  if (open(path, O_RDONLY) < 0) {
  */
  int tmpfd = dup(fd);
  if (tmpfd < 0) {
    errno = EBADF;
    return -1;
  }
  if (close(tmpfd) == -1)
    return -1;

  /* test if newfd is a non-negative and not greater than the maximum */
  // allowable number
  long fmax = open_max();
  if ((long)newfd < 0 || (long)newfd > fmax) {
    errno = EBADF;
    return -1;
  }

  /* test if no need to dup */
  if (newfd == fd)
    return fd;

  /* close newfd after all tests pass, ignore inactive newfd */
  if (close(newfd) == -1 && errno != EBADF)
    return -1;

  return recur(dup(fd), newfd);
}

void test_mydup2(int fdc) {
  long fdmax = open_max();
  printf("OPEN_MAX=%ld\n", fdmax);
  int fd = open("tmp/data/fileio/test_my_dup2", O_RDONLY);
  if (fd == -1)
    perror("Error occured when opening test_my_dup2");
  printf("Open a test file, fd=%d\n", fd);

  int newfd = 0;
  if ((newfd = mydup2(fd, fd)) == -1) {
    my_perror("mydup2(%d, %d)", fd, fd);
  } else {
    assert(newfd == fd);
    printf("mydup2(%d, %d)=%d\n", fd, fd, newfd);
  }
  assert(-1 == mydup2(5, 5)); // invalid fd 5
  assert(EBADF == errno);
  printf("mydup2(%d, %d)=%d, ", 5, 5, -1);
  printf("errno=%d(EBADF)\n", EBADF);
  errno = 0;
  if ((newfd = mydup2(fd, 100)) == -1) {
    my_perror("mydup2(%d, %d)", fd, 100);
  } else {
    assert(newfd == 100);
    printf("mydup2(%d, %d)=%d\n", fd, 100, newfd);
  }
  assert(fdmax-1 == mydup2(fd, fdmax-1));
  printf("mydup2(%d, %ld)=%ld\n", fd, fdmax-1, fdmax-1);

  printf("5 Tests passed.\n");
}

int main(int argc, char *argv[]) {
  int fd = 0;
  if (argc > 1) {
    fd = atoi(argv[1]);
  }
  test_mydup2(fd);
  return 0;
}
