#include "rltapue.h"
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void)
{
  char *path = "tmp/data/fileio/3_6.dat";
  int fd = -1;
  if ((fd = open(path, O_RDWR|O_APPEND)) == -1) { // O_APPEND set in FSF
    my_perror("open file [%s] failed.", path);
  }
  assert(0 == lseek(fd, 0, SEEK_CUR));  /* initial current file offset */
  assert(30 == lseek(fd, 0, SEEK_END)); /* initial file size */
  lseek(fd, 5, SEEK_SET); /* set the offset to 5 */
  char rbuf[128];
  assert(5 == read(fd, rbuf, 5)); /* read 5 bytes, move offset forward by 5 */
  assert(0 == strcmp("56789", rbuf));
  assert(15 == lseek(fd, 5, SEEK_CUR)); /* move forward by another 5, to 15 */
  char *wbuf = "hello";
  // When O_APPEND set in FSF, each write(2) is an atomic appending, i.e.
  // "seek to end and write" or lseek(2) + write(2) as an indivisible op.:
  // 1) retrieve file size from i-node
  // 2) set cur offset the size in file table entry (seek to the end)
  // 3) write buf to the file, i.e. append
  assert(5 == write(fd, wbuf, strlen(wbuf))); /* write wbuf to file, 5 chars */
  assert(35 == lseek(fd, 0, SEEK_CUR)); /* current file offset is 35, not 20 */
  assert(35 == lseek(fd, 0, SEEK_END)); /* current file size is 35, not 30 */
  struct stat st;
  if (fstat(fd, &st) == -1) my_perror("fstat error");
  assert(35 == st.st_size); /* another way to retrieve file size */

  // Below is not about Ex3_6, but additional tests for offset being unchanged
  // after call pread(2)/pwrite(2)
  memset(rbuf, 0, sizeof(rbuf));
  assert(5 == pread(fd, rbuf, 5, 0));
  assert(0 == strcmp("01234", rbuf));
  assert(35 == lseek(fd, 0, SEEK_CUR)); // pread(2) doesn't update offset
  assert(3 == lseek(fd, 3, SEEK_SET));
  assert(strlen("abcde") == pwrite(fd, "abcde", strlen("abcde"), 3));
  assert(3 == lseek(fd, 0, SEEK_CUR));  // pwrite(2) doesn't update offset

  return 0;
}

// NOTE: Atomic Operations:
// 1. pread(2) and pwrite(2)
// pread(2) = lseek(2) + read(2), pwrite(2) = lseek(2) + write(2)
// except that:
// - Atomic, no way to interrupt the two operations (lseek and I/O).
// - Tho current file offset is not updated.
// 2. open(path, O_RDWR | O_CREAT | O_EXCL, mode)
// Atomically test if it exists using open(2) and create using creat(2) if
// it doesn't exist.
// 3. dup2(fd, fd2)
// Atomically close(fd2) and fcntl(fd, F_DUPFD, fd2)


/*
 * Content of 3_6.dat (30 bytes):
0123456789








0123456789
 * ./Ex3_6_append_write_read
 * Content of 3_6.dat (35 bytes):
012abcde89








0123456789
hello
*/
