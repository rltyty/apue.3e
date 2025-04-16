#include "rltapue.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void test_fd3() {
  char *path = "tmp/data/fileio/file1";
  int fd = 0;
  if ((fd = open(path, O_RDWR)) == -1) {
    my_perror("Error occured when open(%s, O_RDWR)", path);
  } else {
    printf("fd=%d\n", fd);
  }
  dup2(fd, 0);
  dup2(fd, 1);
  dup2(fd, 2);
  close(fd);
}

int main(void) { test_fd3(); }
