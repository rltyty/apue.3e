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
#include <unistd.h>

#include "rltapue.h"

int main(void) {
  char *path = "tmp/data/f1.txt";
  int fd = -1;

  if ((fd = open(path, O_RDWR)) == -1) {  // O_RDWR: 0x0002
    my_perror("open file [%s] failed.", path);
  }

  int val;
  if ((val = fcntl(fd, F_GETFL, 0)) < 0)
    perror("fcntl F_GETFL error");
  assert(val == 0x0002);
  set_fl(fd, O_SYNC);                     // O_SYNC: 0x0080 (128)
  if ((val = fcntl(fd, F_GETFL, 0)) < 0)
    perror("fcntl F_GETFL error");
  assert(val == 0x0082);

}

#endif
