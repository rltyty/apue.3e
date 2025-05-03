#include "rltapue.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 4096

/* This test shows that "unbuffered" `read(2)`/`write(2)` means "unbuffered
 * in user space". Buffer or cache are still used during these system
 * opertions in kernel space.
 *
 * In the test, `write(2)` with `O_SYNC` takes about four times system time
 * as long as without `O_SYNC`, while user time is not affected.
 *
 * User buffer size adjustment is to change the number of calls of
 * `read(2)/`write(2)`, which could affect user time.
 *
 * About discrepancy in real vs. sys
 * - On macOS, the sys time increase (0.01s → 0.10s) roughly matches the
 *   real time increase (0.02s → 0.14s)
 * - On Linux, the sys time increase (0.008s → 0.123s) is much smaller than
 *   the real time increase (0.012s → 2.405s)
 *
 * - On Linux (ext4/xfs/btrfs), O_SYNC is stricter and often bypasses more
 *   aggressive caching layers, forcing data all the way to the physical disk.
 *   This involves more synchronous operations (e.g., waiting for disk
 *   acknowledgments), which significantly increases latency.
 * - On macOS (APFS/HFS+), the O_SYNC flag ensures that data is written to
 *   disk before the write() system call returns, but the filesystem may
 *   still optimize this process in ways that reduce the wall-clock time
 *   impact (e.g., leveraging the NVMe controller's write buffers or the
 *   disk's own cache).
 *
 * In summary, Linux's O_SYNC is more "honest" and forces writes to disk
 * immediately, exposing the full disk latency in real time.
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
}

/*
 * Prepare 8mb.dat

dd if=/dev/random of=8mb.dat bs=4096 count=2048

 * Test no-sync/sync with different i/o buffer size

 * On macOS:
> /usr/bin/time ./Debug/fileio/Ex3_1_read_write_2 8192 < 8mb.dat
        0.02 real         0.00 user         0.01 sys
> /usr/bin/time ./Debug/fileio/Ex3_1_read_write_2 8192 sync < 8mb.dat
        0.14 real         0.00 user         0.10 sys

> /usr/bin/time ./Debug/fileio/Ex3_1_read_write_2 4096 < 8mb.dat
        0.04 real         0.00 user         0.03 sys
> /usr/bin/time ./Debug/fileio/Ex3_1_read_write_2 4096 sync < 8mb.dat
        0.21 real         0.00 user         0.16 sys

> /usr/bin/time ./Debug/fileio/Ex3_1_read_write_2 1024 < 8mb.dat
        0.07 real         0.00 user         0.05 sys
> /usr/bin/time ./Debug/fileio/Ex3_1_read_write_2 1024 sync < 8mb.dat
        0.50 real         0.00 user         0.30 sys

 * On Linux:
❯ /usr/bin/time -f "%e real     %U user     %S sys" ./Debug/fileio/Ex3_1_read_write_2 8192 < 8mb.dat
0.00 real     0.00 user     0.00 sys
❯ /usr/bin/time -f "%e real     %U user     %S sys" ./Debug/fileio/Ex3_1_read_write_2 8192 sync < 8mb.dat
2.44 real     0.00 user     0.11 sys

❯ /usr/bin/time -f "%e real     %U user     %S sys" ./Debug/fileio/Ex3_1_read_write_2 4096 < 8mb.dat
0.00 real     0.00 user     0.00 sys
❯ /usr/bin/time -f "%e real     %U user     %S sys" ./Debug/fileio/Ex3_1_read_write_2 4096 sync < 8mb.dat
4.70 real     0.00 user     0.22 sys

❯ /usr/bin/time -f "%e real     %U user     %S sys" ./Debug/fileio/Ex3_1_read_write_2 1024 < 8mb.dat
0.02 real     0.00 user     0.01 sys
❯ /usr/bin/time -f "%e real     %U user     %S sys" ./Debug/fileio/Ex3_1_read_write_2 1024 sync < 8mb.dat
16.90 real     0.02 user     0.66 sys
 */
