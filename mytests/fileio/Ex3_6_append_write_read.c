#include "rltapue.h"
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
  char *path = "tmp/data/fileio/3_6.dat";
  int fd = -1;
  if ((fd = open(path, O_RDWR|O_APPEND)) == -1) {
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
  assert(5 == write(fd, wbuf, strlen(wbuf))); /* write wbuf to file, 5 chars */
  assert(35 == lseek(fd, 0, SEEK_CUR)); /* current file offset is 35, not 20 */
  assert(35 == lseek(fd, 0, SEEK_END)); /* current file size is 35, not 30 */
  return 0;
}
