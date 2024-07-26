#include "apue.h"
#include <errno.h>
#include <unistd.h>

void test_arg(int argc, char *argv[]);
void test_posix_version();
void test_xsi_version();
void test_posix_asynchronous_io();
void test_posix_aync_io();

/* macOS: SUSv3 */
int main(int argc, char *argv[]) {
  test_arg(argc, argv);
  test_posix_version();
  test_xsi_version();
  test_posix_asynchronous_io();
  test_posix_aync_io();
  exit(0);
}

void test_arg(int argc, char *argv[]) {
  if (argc == 1)
    err_quit("usage: test_misc <any arg>");
}

void test_posix_version() {
  long posix_version = 0;
  posix_version = sysconf(_SC_VERSION); /* SUSv3: 200112, SUSv4: 200809 */
  printf("posix_version=%ld\n", posix_version);
}

void test_xsi_version() {
  long xsi_version = 0;
  xsi_version = sysconf(_SC_XOPEN_VERSION); /* SUSv3: 600, SUSv4: 700 */
  printf("xsi_version=%ld\n", xsi_version);
}

void test_posix_asynchronous_io() {
  long asynchronous_io = 0;
  asynchronous_io = sysconf(_SC_ASYNCHRONOUS_IO);
  printf("asynchronous_io=%ld\n", asynchronous_io);
}

void test_posix_aync_io() {
  long async_io = 0;
  async_io = pathconf("./test_misc.c", _PC_ASYNC_IO);
  printf("async_io of a file = %ld\n", async_io);
}
