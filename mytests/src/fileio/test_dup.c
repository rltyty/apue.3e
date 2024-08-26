#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void test_dup(void);
void test_dup_open_dev_fd_oflags();
int get_dev_fd_path(char **, int);
void open_dev_fd_rdonly_rdwr(int);
void print_sep(void);

int main(void) {
  test_dup();
  test_dup_open_dev_fd_oflags();
  return 0;
}

void test_dup(void) {
  /* int fdts = getdtablesize(); */ /* deprecated, use sysconf(_SC_OPEN_MAX) */
  long fdts = sysconf(_SC_OPEN_MAX);
#ifdef MACOS
  assert(256 == fdts);
#elif defined LINUX
  assert(1024 == fdts);
#endif
  printf("getdtablesize() is deprecated, sysconf(_SC_OPEN_MAX)=%ld\n", fdts);

  int fd = dup(1);
  assert(3 == fd);
  printf("dup(1)=%d\n", fd);

  fd = dup(fd);
  assert(4 == fd);
  printf("dup(3)=%d\n", fd);

  fd = dup2(fd, 6);
  assert(fd == 6);
  printf("dup2(4, 6)=%d\n", fd);

  fd = dup(fd);
  assert(fd == 5);
  printf("dup(6)=%d\n", fd);
}

void test_dup_open_dev_fd_oflags() {

  /* test oflags and compare Linux implementation to Unix's about /dev/fd/n */

  /*
      O_RDONLY        0x0000
      O_WRONLY        0x0001
      O_RDWR          0x0002
      O_ACCMODE       0x0003
   */

  int fl = fcntl(STDIN_FILENO, F_GETFL);
  int amode = fl & O_ACCMODE;
  printf("fnctl(STDIN_FILENO, F_GETFL)=%d & O_ACCMODE = %d\n", fl, amode);

  fl = fcntl(STDOUT_FILENO, F_GETFL);
  amode = fl & O_ACCMODE;
  printf("fnctl(STDOUT_FILENO, F_GETFL)=%d & O_ACCMODE = %d\n", fl, amode);

  int fd = 5;
  fl = fcntl(fd, F_GETFL);
  amode = fl & O_ACCMODE;
  printf("fnctl(%d, F_GETFL)=%d & O_ACCMODE = %d\n", fd, fl, amode);

  print_sep();

  open_dev_fd_rdonly_rdwr(STDIN_FILENO);
  print_sep();
  open_dev_fd_rdonly_rdwr(STDOUT_FILENO);
  print_sep();

  if ((fd = open("./data/fileio/file1", O_RDWR)) < 0) {
    perror("open file1 failed.");
  } else {
    printf("open: ./data/fileio/file1 with O_RDWR, fd=%d\n", fd);
  }
  open_dev_fd_rdonly_rdwr(fd);

  print_sep();

  if ((fd = open("./data/fileio/file1", O_RDONLY)) < 0) {
    perror("open file1 failed.");
  } else {
    printf("open: ./data/fileio/file1 with O_RDONLY, fd=%d\n", fd);
  }
  open_dev_fd_rdonly_rdwr(fd);
}

int get_dev_fd_path(char **path, int fd) {
  if (asprintf(path, "/dev/fd/%d", fd) == -1) {
    perror("asprintf: failed to allocate space for path.");
    return -1;
  }
  return 0;
}

void open_dev_fd_rdonly_rdwr(int fd) {
  int newfd;
  char *path;
  if (get_dev_fd_path(&path, fd) == 0) {
    if ((newfd = open(path, O_RDONLY)) < 0) {
      char *msg;
      if (asprintf(&msg, "open [%s] with O_RDONLY failed.\n", path) == -1) {
        perror("asprintf: failed to allocate space for msg.");
      }
      perror(msg);
    } else {
      printf("open [%s] with O_RDONLY succeeded, fd=%d.\n", path, newfd);
      int fl = fcntl(newfd, F_GETFL);
      int amode = fl & O_ACCMODE;
      printf("fnctl(%d, F_GETFL)=%d & O_ACCMODE = %d\n", newfd, fl, amode);
    }

    if ((newfd = open(path, O_RDWR)) < 0) {
      char *msg;
      if (asprintf(&msg, "open [%s] with O_RDWR failed.\n", path) == -1) {
        perror("asprintf: failed to allocate space for msg.");
      }
      perror(msg);
    } else {
      printf("open [%s] with O_RDWR succeeded, fd=%d.\n", path, newfd);
      int fl = fcntl(newfd, F_GETFL);
      int amode = fl & O_ACCMODE;
      printf("fnctl(%d, F_GETFL)=%d & O_ACCMODE = %d\n", newfd, fl, amode);
    }
  }
}

void print_sep(void) { printf("===================\n"); }

/*
 * Debian 12 64-bit
 *
> ./Release/fileio/test_dup
getdtablesize() is deprecated, sysconf(_SC_OPEN_MAX)=1024
dup(1)=3
dup(3)=4
dup2(4, 6)=6
dup(6)=5
fnctl(STDIN_FILENO, F_GETFL)=2 & O_ACCMODE = 2
fnctl(STDOUT_FILENO, F_GETFL)=2 & O_ACCMODE = 2
fnctl(5, F_GETFL)=2 & O_ACCMODE = 2
===================
open [/dev/fd/0] with O_RDONLY succeeded, fd=7.
fnctl(7, F_GETFL)=32768 & O_ACCMODE = 0
open [/dev/fd/0] with O_RDWR succeeded, fd=8.
fnctl(8, F_GETFL)=32770 & O_ACCMODE = 2
===================
open [/dev/fd/1] with O_RDONLY succeeded, fd=9.
fnctl(9, F_GETFL)=32768 & O_ACCMODE = 0
open [/dev/fd/1] with O_RDWR succeeded, fd=10.
fnctl(10, F_GETFL)=32770 & O_ACCMODE = 2
===================
open: ./data/fileio/file1 with O_RDWR, fd=11
open [/dev/fd/11] with O_RDONLY succeeded, fd=12.
fnctl(12, F_GETFL)=32768 & O_ACCMODE = 0
open [/dev/fd/11] with O_RDWR succeeded, fd=13.
fnctl(13, F_GETFL)=32770 & O_ACCMODE = 2
===================
open: ./data/fileio/file1 with O_RDONLY, fd=14
open [/dev/fd/14] with O_RDONLY succeeded, fd=15.
fnctl(15, F_GETFL)=32768 & O_ACCMODE = 0
open [/dev/fd/14] with O_RDWR succeeded, fd=16.
fnctl(16, F_GETFL)=32770 & O_ACCMODE = 2
 *
 *
 * Raspbian Pi OS 11 64-bit
 *
> ./Release/fileio/test_dup
getdtablesize() is deprecated, sysconf(_SC_OPEN_MAX)=1024
dup(1)=3
dup(3)=4
dup2(4, 6)=6
dup(6)=5
fnctl(STDIN_FILENO, F_GETFL)=1026 & O_ACCMODE = 2
fnctl(STDOUT_FILENO, F_GETFL)=1026 & O_ACCMODE = 2
fnctl(5, F_GETFL)=1026 & O_ACCMODE = 2
===================
open [/dev/fd/0] with O_RDONLY succeeded, fd=7.
fnctl(7, F_GETFL)=0 & O_ACCMODE = 0
open [/dev/fd/0] with O_RDWR succeeded, fd=8.
fnctl(8, F_GETFL)=2 & O_ACCMODE = 2
===================
open [/dev/fd/1] with O_RDONLY succeeded, fd=9.
fnctl(9, F_GETFL)=0 & O_ACCMODE = 0
open [/dev/fd/1] with O_RDWR succeeded, fd=10.
fnctl(10, F_GETFL)=2 & O_ACCMODE = 2
===================
open: ./data/fileio/file1 with O_RDWR, fd=11
open [/dev/fd/11] with O_RDONLY succeeded, fd=12.
fnctl(12, F_GETFL)=0 & O_ACCMODE = 0
open [/dev/fd/11] with O_RDWR succeeded, fd=13.
fnctl(13, F_GETFL)=2 & O_ACCMODE = 2
===================
open: ./data/fileio/file1 with O_RDONLY, fd=14
open [/dev/fd/14] with O_RDONLY succeeded, fd=15.
fnctl(15, F_GETFL)=0 & O_ACCMODE = 0
open [/dev/fd/14] with O_RDWR succeeded, fd=16.
fnctl(16, F_GETFL)=2 & O_ACCMODE = 2
 *
 *
 * macOS 12.7 64-bit
 *
> ./Release/fileio/test_dup
getdtablesize() is deprecated, sysconf(_SC_OPEN_MAX)=256
dup(1)=3
dup(3)=4
dup2(4, 6)=6
dup(6)=5
fnctl(STDIN_FILENO, F_GETFL)=65538 & O_ACCMODE = 2
fnctl(STDOUT_FILENO, F_GETFL)=65538 & O_ACCMODE = 2
fnctl(5, F_GETFL)=65538 & O_ACCMODE = 2
===================
open [/dev/fd/0] with O_RDONLY succeeded, fd=7.
fnctl(7, F_GETFL)=65538 & O_ACCMODE = 2
open [/dev/fd/0] with O_RDWR succeeded, fd=8.
fnctl(8, F_GETFL)=65538 & O_ACCMODE = 2
===================
open [/dev/fd/1] with O_RDONLY succeeded, fd=9.
fnctl(9, F_GETFL)=65538 & O_ACCMODE = 2
open [/dev/fd/1] with O_RDWR succeeded, fd=10.
fnctl(10, F_GETFL)=65538 & O_ACCMODE = 2
===================
open: ./data/fileio/file1 with O_RDWR, fd=11
open [/dev/fd/11] with O_RDONLY succeeded, fd=12.
fnctl(12, F_GETFL)=2 & O_ACCMODE = 2
open [/dev/fd/11] with O_RDWR succeeded, fd=13.
fnctl(13, F_GETFL)=2 & O_ACCMODE = 2
===================
open: ./data/fileio/file1 with O_RDONLY, fd=14
open [/dev/fd/14] with O_RDONLY succeeded, fd=15.
fnctl(15, F_GETFL)=0 & O_ACCMODE = 0
open [/dev/fd/14] with O_RDWR failed.
: Permission denied
 */
