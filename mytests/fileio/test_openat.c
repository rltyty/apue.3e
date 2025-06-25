#include "rltapue.h"
#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>

void test_openat()
{
  char *d1 = "tmp/data/fileio/openat1";
  int fd_d1;
  // open a directory
  if ((fd_d1 = open(d1, O_RDONLY | O_DIRECTORY)) < 0)
    my_perror("Open dir failed.");
  int fd_f1;
  // open a file under the opened directory
  if ((fd_f1 = openat(fd_d1, "file1", O_RDONLY)) < 0)
    my_perror("Open file failed."); 
  char buf[BUFSIZ];
  int n;
  while ((n = read(fd_f1, buf, BUFSIZ)) > 0) {
    if (write(STDOUT_FILENO, buf, n) != n)
      my_perror("Error occurred when write file.");
  }
}

int main(int argc, char **args)
{
  test_openat();
  return 0;
}
