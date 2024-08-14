#include "apue.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>

int recur(int, int, long);

int mydup2(int fd, int newfd) {
  // test if fd is an active, valid file descriptor
  char path[128];
  snprintf(path, 128, "/dev/fd/%d", fd);
  if (open(path, O_RDONLY) < 0) {
    errno = EBADF;
    return -1;
  }

  // test if newfd is an non-negative and not greater than the maximum allowable
  // number
  long fmax = open_max();
  if ((long)newfd < 0 || (long)newfd > fmax) {
    errno = EBADF;
    return -1;
  }

  if (newfd == fd)
    return fd;

  return recur(dup(fd), newfd, fmax);
}

int recur(int fd, int newfd, long fmax)
{
  if (fd == -1)
    return -1;

  int ret = -1;
  if (fd != newfd) {
    ret = recur(dup(fd), newfd, fmax);
  } else {
    return fd;
  }
  if (close(fd) != 0) {
    err_sys("Failed to close(%d)", fd);
  }
  return ret;
}

void test_mydup2()
{
  int fd = open("data/fileio/test_my_dup2", O_RDONLY);
  if (fd == -1) err_sys("Error occured when opening test_my_dup2");
  printf("Open a test file, fd=%d\n", fd);

  int newfd = 0;
  if ((newfd = mydup2(fd, fd)) == -1) {
    err_sys("mydup2(%d, %d)", fd, fd);
  } else {
    assert(newfd == fd);
    printf("mydup2(%d, %d)=%d\n", fd, fd, newfd);
  }
  assert(-1 == mydup2(5, 5));
  assert(EBADF == errno);
  printf("mydup2(%d, %d)=%d\n", 5, 5, -1);
  printf("errno=%d(EBADF)\n", EBADF);
  errno = 0;
  if ((newfd = mydup2(fd, 100)) == -1) {
    err_sys("mydup2(%d, %d)", fd, 100);
  } else {
    assert(newfd == 100);
    printf("mydup2(%d, %d)=%d\n", fd, 100, newfd);
  }
  printf("3 Tests passed.\n");
}

int main(void)
{
  test_mydup2();
  return 0;
}
