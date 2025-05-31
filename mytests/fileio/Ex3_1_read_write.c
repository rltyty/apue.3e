#include "rltapue.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* This test shows that "unbuffered" `read(2)`/`write(2)` means "unbuffered
 * in user space". Buffer or cache are still used during these system
 * opertions in kernel space.
 *
 * In the test, `write(2)` with `O_SYNC` takes about four times system time
 * as long as without `O_SYNC`, while user time is not affected.
 *
 * User buffer size adjustment is to change the number of calls of
 * `read(2)/`write(2)`, which could affect user time.
 */

/* .a.out [bufsize [sync]] */

int main(int argc, char *argv[]) {
  int bufsize = BUFSIZE;
  if (argc > 1) {
    int t = atoi(argv[1]);
    if (t > 0)
      bufsize = t;
  }
  char buf[bufsize];
  int n;

#ifdef __linux__
  // Linux doesn't support modify fd flag O_SYNC on the fly. Set O_SYNC
  // when open(2).
  int fd;
  char *path = "./tmp/copy.dat";
  int oflag = O_CREAT | O_WRONLY | O_TRUNC;
  if (argc > 2 && strcmp(argv[2], "sync") == 0)
    oflag |= O_SYNC;
  if ((fd = open(path, oflag, 0333)) < 0)
    my_perror("Open file [%s] failed.", path);
  while ((n = read(STDIN_FILENO, buf, bufsize)) > 0) {
    if (write(fd, buf, n) != n)
      perror("Error occurred when write file.");
  }
#else
  if (argc > 2 && strcmp(argv[2], "sync") == 0)
    set_fl(STDOUT_FILENO, O_SYNC);

  while ((n = read(STDIN_FILENO, buf, bufsize)) > 0) {
    if (write(STDOUT_FILENO, buf, n) != n)
      perror("Error occurred when write file.");
  }
#endif
}

/*
 * Prepare 8mb.dat

dd if=/dev/random of=8mb.dat bs=4096 count=2048

 * Test no-sync/sync with different i/o buffer size

 * On macOS:
> /usr/bin/time ./Debug/fileio/Ex3_1_read_write 8192 < 8mb.dat > copy.dat
        0.02 real         0.00 user         0.01 sys
> /usr/bin/time ./Debug/fileio/Ex3_1_read_write 8192 sync < 8mb.dat > copy.dat
        0.14 real         0.00 user         0.10 sys

> /usr/bin/time ./Debug/fileio/Ex3_1_read_write 4096 < 8mb.dat > copy.dat
        0.03 real         0.00 user         0.03 sys
> /usr/bin/time ./Debug/fileio/Ex3_1_read_write 4096 sync < 8mb.dat > copy.dat
        0.21 real         0.00 user         0.15 sys

> /usr/bin/time ./Debug/fileio/Ex3_1_read_write 1024 < 8mb.dat > copy.dat
        0.07 real         0.00 user         0.05 sys
> /usr/bin/time ./Debug/fileio/Ex3_1_read_write 1024 sync < 8mb.dat > copy.dat
        0.49 real         0.00 user         0.29 sys

 * On Linux:

 ❯ /usr/bin/time -f "%e real     %U user     %S sys" ./Debug/fileio/Ex3_1_read_write 8192 < 8mb.dat
0.00 real     0.00 user     0.00 sys
❯ /usr/bin/time -f "%e real     %U user     %S sys" ./Debug/fileio/Ex3_1_read_write 8192 sync < 8mb.dat
2.43 real     0.00 user     0.11 sys

❯ /usr/bin/time -f "%e real     %U user     %S sys" ./Debug/fileio/Ex3_1_read_write 4096 < 8mb.dat
0.00 real     0.00 user     0.00 sys
❯ /usr/bin/time -f "%e real     %U user     %S sys" ./Debug/fileio/Ex3_1_read_write 4096 sync < 8mb.dat
4.61 real     0.00 user     0.15 sys

❯ /usr/bin/time -f "%e real     %U user     %S sys" ./Debug/fileio/Ex3_1_read_write 1024 < 8mb.dat
0.02 real     0.00 user     0.01 sys
❯ /usr/bin/time -f "%e real     %U user     %S sys" ./Debug/fileio/Ex3_1_read_write 1024 sync < 8mb.dat
17.00 real     0.02 user     0.66 sys

 */
