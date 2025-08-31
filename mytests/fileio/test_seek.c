#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFSIZE 4096

off_t showcfo(int, const char*);
off_t readnb(int, int);
off_t fsize(int);

/*
 * 1. If open(2) with O_WRONLY, then read(2) reads nothing.
 * 2. If open(2) with O_APPEND, a corresponding flag (append) in file status
 *    flags of the file table entry will be set. The current file offset is
 *    still at the beginning. Later, every write(2) call will firstly set the
 *    current file offset to the current file size from the i-node table
 *    entry. This forces every write(2) to be appended to the current end of
 *    file.
 * 3. O_APPEND doesn't affect read(2). open(2) with O_RDWR|O_APPEND
 */
int main(void) {
  int fd;
  char *fname = "tmp/data/fileio/seek.dat";
  char wbuf[BUFFSIZE];
  char wbuf2[BUFFSIZE];
  strcpy(wbuf, "hello, world!");
  strcpy(wbuf2, ", Messi!");
  if ((fd = open(fname, O_RDWR|O_APPEND)) > 0) {
    assert(0 == showcfo(fd, "Open with O_RDWR|O_APPEND"));
#ifdef _GNU_SOURCE
    printf("file size: %ld\n", fsize(fd));
#else
    printf("file size: %lld\n", fsize(fd));
#endif
    assert(30 == fsize(fd));
    printf("wbuf:[%s]\n", wbuf);
    off_t size = fsize(fd);
    assert(size + write(fd, wbuf, 15) == \
                                showcfo(fd, "After write at most 15 bytes"));
    char *msg;
#ifdef _GNU_SOURCE
    asprintf(&msg, "After reset current offset to %ld",
                                                    lseek(fd, 20, SEEK_SET));
#else
    asprintf(&msg, "After reset current offset to %lld",
                                                    lseek(fd, 20, SEEK_SET));
#endif
    assert(20 == showcfo(fd, msg));
    assert(25 == readnb(fd, 5));
    asprintf(&msg, "After write wbuf2:[%s]", wbuf2);
    assert(fsize(fd) + write(fd, wbuf2, strlen(wbuf2)) == showcfo(fd, msg));
  } else {
    printf("Error occurred when open file [%s]\n", fname);
  }
  return 0;
}

off_t fsize(int fd)
{
    return lseek(fd, 0, SEEK_END);
}

off_t showcfo(int fd, const char *msg)
{
    off_t cfo = lseek(fd, 0, SEEK_CUR);
#ifdef _GNU_SOURCE
    printf("%s, cfo=[%ld]\n", msg, cfo);
#else
    printf("%s, cfo=[%lld]\n", msg, cfo);
#endif
    return cfo;
}

off_t readnb(int fd, int n)
{
  char rbuf[BUFFSIZE];
  read(fd, rbuf, n);
  rbuf[n] = '\0';
  char *msg; 
  asprintf(&msg, "After read %d bytes: [%s]", n, rbuf);
  return showcfo(fd, msg);
}
