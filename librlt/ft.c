#include "rltapue.h"
#include <sys/stat.h>

int get_ft(const char *p, char **type) {
  struct stat buf;
  if (lstat(p, &buf) < 0) { // just stat the link itself
    my_perror("lstat");
    return -1;
  }
  if (S_ISREG(buf.st_mode))
    *type = "regular";
  else if (S_ISDIR(buf.st_mode))
    *type = "directory";
  else if (S_ISCHR(buf.st_mode))
    *type = "character special";
  else if (S_ISBLK(buf.st_mode))
    *type = "block special";
  else if (S_ISFIFO(buf.st_mode))
    *type = "fifo";
  else if (S_ISLNK(buf.st_mode))
    *type = "symbolic link";
  else if (S_ISSOCK(buf.st_mode))
    *type = "socket";
  else
    *type = "** unknown mode **";
  return buf.st_mode & S_IFMT;
}

#ifdef test_ft
#undef test_ft
#include <stddef.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
  if (argc <= 1) {
    printf("Usage: %s <file>\n", argv[0]);
    return -1;
  }
  char *type = NULL;
  for (int i = 1; i < argc; i++) {
    int t = get_ft(argv[i], &type);
    printf("file type: %s, type bit: %#o\n", type, t);
  }
  return 0;
}
/*
 * Test on macOS:
:!./ft /etc/fstab /etc/ /dev/tty /dev/disk0 /dev/stdin /var/run/usbmuxd tmp/data/fifo
file type: regular, type bit: 0100000
file type: directory, type bit: 040000
file type: character special, type bit: 020000
file type: block special, type bit: 060000
file type: symbolic link, type bit: 0120000
file type: socket, type bit: 0140000
file type: fifo, type bit: 010000
 */
#endif

