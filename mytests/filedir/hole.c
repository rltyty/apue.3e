/* create sparse file */

/**
 * NOTE:
   1. Create a real 1GB file filling with zeros
   ```sh
   dd if=/dev/zero of=real1G bs=1G count=1
   ```
   2. Create a sparse1G file with a hole
   ```sh
   dd if=/dev/zero of=sparse1G bs=1 count=0 seek=1G
   ```
   3. Logical size and disk usage
   ```sh
   ls -lhs hole1G sparse1G real1G
   1.0G -rw-r--r-- 1 gpanda staff 1.0G Aug  7 12:29 real1G
      0 -rw-r--r-- 1 gpanda staff 1.0G Aug  7 12:25 sparse1G
   8.0K -rw-r--r-- 1 gpanda staff 1.1G Jun 27 07:39 hole1G
   ```
   `ls -l`: shows logical size, st_size
   `ls -s`: shows disk usage, st_blocks
   By default, 1 block = 512B, so 16 blocks = 8.0KB

   4. Use `stat file` to show file logical size and blocks usage
 */

#include "rltapue.h"
#include <stddef.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <unistd.h>

#define HOLE_1K     (1024UL)
#define HOLE_1M     (1024UL * 1024UL)
#define HOLE_10M    (1024UL * 1024UL * 10UL)
#define HOLE_100M   (1024UL * 1024UL * 100UL)
#define HOLE_1G     (1024UL * 1024UL * 1024UL)

const char START_MARK[] = "ABCDEFGHIJ";
const char END_MARK[] = "abcdefghij";

int
main(int argc, char *argv[])
{
  int rc = 1;

  if (argc < 2) {
    my_perror("Usage: %s <filename> [<hole size>]", argv[0]);
    return 1;
  }
  const int fd = open(argv[1], O_CREAT|O_WRONLY|O_TRUNC, 0644);
  if (fd < 0) {
    my_perror("Failed to create %s.", argv[1]);
    return 1;
  }

  size_t hsize;
  if (argc > 2) {
    hsize = atoi(argv[2]);
  } else {
    hsize = HOLE_1G;
  }

  if (write(fd, START_MARK, sizeof(START_MARK)) < sizeof(START_MARK)) {
    my_perror("write");
    goto done;
  }


  if (lseek(fd, hsize, SEEK_CUR) < 0) {
    my_perror("lseek");
    goto done;
  }

  if (write(fd, END_MARK, sizeof(START_MARK)) < sizeof(START_MARK)) {
    my_perror("write");
    goto done;
  }

  rc = 0;

done:
  close(fd);
  return rc;
}
