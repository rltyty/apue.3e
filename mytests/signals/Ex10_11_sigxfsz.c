/**
 * Under Linux, macOS, and Solaris, as long as the current file size is below
 * the limit, `write(2)` will never cross the boundary. The kernel lets the
 * file grow right up to the limit and then stop. So it won't send the SIGXFSZ
 * signal and the handler won't be called.
 *
 * Suppose the file size is 500, the limit is 512, another `write(2)` of 100
 * bytes will return a count of 12 as the file size reaches the limit.
 *
 * When the file size reaches the limit and if we attempt an additional write
 * from the end of the file, we will receive SIGXFSZ and `write(2)` will return
 * -1 with errno set to EFBIG.
 */

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

#include "rltapue.h"
#define BUFFSIZE 100
#define FSIZE_LIM 512

void sig_hand(int signo) {
  fprintf(stderr, "Signal[%d] is caught: %s\n", signo, strsignal(signo));
}

rlim_t my_setrlim(int name, rlim_t cur) {
  struct rlimit lim;
  if (getrlimit(name, &lim) < 0) {
    my_perror("error: getrlimit(RLIMIT_FSIZE, ...)");
  }
  rlim_t old = lim.rlim_cur;
  lim.rlim_cur = cur;
  if (setrlimit(name, &lim) < 0) {
    my_perror("error: setrlimit(RLIMIT_FSIZE, ...): %ld", cur);
  }
  return old;
}

void ulimit_test() {
  size_t nr, nw;
  char buf[BUFFSIZE];
  while ((nr = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
    if ((nw = write(STDOUT_FILENO, buf, nr)) != nr) {
      my_perror_ret("error: write: nw = %ld", nw);
    }
  if (nr < 0) my_perror("error: read: nr = %ld", nr);
}

void setrlimit_test() {
  rlim_t old_fsz = my_setrlim(RLIMIT_FSIZE, FSIZE_LIM);
  size_t nr, nw;
  char buf[BUFFSIZE];
  while ((nr = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
    if ((nw = write(STDOUT_FILENO, buf, nr)) != nr) {
      my_perror_ret("error: write: nw = %ld", nw);
    }
  if (nr < 0) my_perror("error: read: nr = %ld", nr);
  assert(FSIZE_LIM == my_setrlim(RLIMIT_FSIZE, old_fsz));
}

int main(int argc, char *argv[]) {
  // 1. Install signal handler
  struct sigaction act = {0}, oact = {0};
  act.sa_handler = sig_hand;
  if (sigaction(SIGXFSZ, &act, &oact) < 0) {
    my_perror("error: sigaction()");
  }

  // 2. Test event
  if (argc == 2) {
    fputs("Setup ulimit via `ulimimt -f 1`.\n", stderr);
    ulimit_test();
  } else {
    fputs("Setup rlimit via program.\n", stderr);
    setrlimit_test();
  }

  // 3. Restore signal action
  if (sigaction(SIGXFSZ, &oact, NULL) < 0) {
    my_perror("error: sigaction: restore");
  }
  return 0;
}

/*

## --------------------------------------------------------------
## Prepare fixed size file
> dd if=/dev/random of=./512B bs=1B count=512
512+0 records in
512+0 records out
512 bytes copied, 0.002261 s, 226 kB/s
> dd if=/dev/random of=./513B bs=1B count=513
513+0 records in
513+0 records out
513 bytes copied, 0.002686 s, 191 kB/s
> dd if=/dev/random of=1K bs=1K count=1
1+0 records in
1+0 records out
1024 bytes (1.0 kB, 1.0 KiB) copied, 0.000162 s, 6.3 MB/s
> ll
total 12
drwxr-xr-x   5 gpanda staff  160 Sep 18 11:40 ./
drwxr-xr-x 295 gpanda staff 9440 Sep 18 07:18 ../
-rw-r--r--   1 gpanda staff 1024 Sep 18 11:40 1K
-rw-r--r--   1 gpanda staff  512 Sep 18 11:40 512B
-rw-r--r--   1 gpanda staff  513 Sep 18 11:40 513B

> ulimit -a | grep "\-f"
-f: file size (blocks)              unlimited

## --------------------------------------------------------------
## Test 1: Setup rlimit via program.
> ~/wksp/apue.3e/mytests/Debug/signals/Ex10_11_sigxfsz < ./512B > copy
Setup rlimit via program.
> ll
total 16
drwxr-xr-x   6 gpanda staff  192 Sep 18 18:45 ./
drwxr-xr-x 295 gpanda staff 9440 Sep 18 07:18 ../
-rw-r--r--   1 gpanda staff 1024 Sep 18 11:40 1K
-rw-r--r--   1 gpanda staff  512 Sep 18 11:40 512B
-rw-r--r--   1 gpanda staff  513 Sep 18 11:40 513B
-rw-r--r--   1 gpanda staff  512 Sep 18 18:45 copy

> ~/wksp/apue.3e/mytests/Debug/signals/Ex10_11_sigxfsz < ./513B > copy
Setup rlimit via program.
error: write: nw = 12: Undefined error: 0

> ~/wksp/apue.3e/mytests/Debug/signals/Ex10_11_sigxfsz < ./1K > copy
Setup rlimit via program.
error: write: nw = 12: Undefined error: 0
Signal[25] is caught: Filesize limit exceeded: 25
error: write: nw = -1: File too large
Signal[25] is caught: Filesize limit exceeded: 25
error: write: nw = -1: File too large
Signal[25] is caught: Filesize limit exceeded: 25
error: write: nw = -1: File too large
Signal[25] is caught: Filesize limit exceeded: 25
error: write: nw = -1: File too large
Signal[25] is caught: Filesize limit exceeded: 25
error: write: nw = -1: File too large

## --------------------------------------------------------------
## Test 2: Setup ulimit via `ulimimt -f 1`.

> ulimit -f 1
> ulimit -a | grep "\-f"
-f: file size (blocks)              1
> ~/wksp/apue.3e/mytests/Debug/signals/Ex10_11_sigxfsz 1 < ./512B > copy
Setup ulimit via `ulimimt -f 1`.
> ll
total 16
drwxr-xr-x   6 gpanda staff  192 Sep 18 18:45 ./
drwxr-xr-x 295 gpanda staff 9440 Sep 18 07:18 ../
-rw-r--r--   1 gpanda staff 1024 Sep 18 11:40 1K
-rw-r--r--   1 gpanda staff  512 Sep 18 11:40 512B
-rw-r--r--   1 gpanda staff  513 Sep 18 11:40 513B
-rw-r--r--   1 gpanda staff  512 Sep 18 18:53 copy
> ~/wksp/apue.3e/mytests/Debug/signals/Ex10_11_sigxfsz 1 < ./513B > copy
Setup ulimit via `ulimimt -f 1`.
error: write: nw = 12: Undefined error: 0
> ~/wksp/apue.3e/mytests/Debug/signals/Ex10_11_sigxfsz 1 < ./1K > copy
Setup ulimit via `ulimimt -f 1`.
error: write: nw = 12: Undefined error: 0
Signal[25] is caught: Filesize limit exceeded: 25
error: write: nw = -1: File too large
Signal[25] is caught: Filesize limit exceeded: 25
error: write: nw = -1: File too large
Signal[25] is caught: Filesize limit exceeded: 25
error: write: nw = -1: File too large
Signal[25] is caught: Filesize limit exceeded: 25
error: write: nw = -1: File too large
Signal[25] is caught: Filesize limit exceeded: 25
error: write: nw = -1: File too large
> ll
total 16
drwxr-xr-x   6 gpanda staff  192 Sep 18 18:45 ./
drwxr-xr-x 295 gpanda staff 9440 Sep 18 07:18 ../
-rw-r--r--   1 gpanda staff 1024 Sep 18 11:40 1K
-rw-r--r--   1 gpanda staff  512 Sep 18 11:40 512B
-rw-r--r--   1 gpanda staff  513 Sep 18 11:40 513B
-rw-r--r--   1 gpanda staff  512 Sep 18 18:54 copy
 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
