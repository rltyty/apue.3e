#include <assert.h>
#include <stdio.h>
#include <sys/_types/_s_ifmt.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "rltapue.h"

void get_ft(char *p) {
  struct stat buf;
  char *type;
  if (lstat(p, &buf) < 0) {
    my_perror("lstat");
    return;
  }
  if (S_ISREG(buf.st_mode))
    type = "regular";
  else if (S_ISDIR(buf.st_mode))
    type = "directory";
  else if (S_ISCHR(buf.st_mode))
    type = "character special";
  else if (S_ISBLK(buf.st_mode))
    type = "block special";
  else if (S_ISFIFO(buf.st_mode))
    type = "fifo";
  else if (S_ISLNK(buf.st_mode))
    type = "symbolic link";
  else if (S_ISSOCK(buf.st_mode))
    type = "socket";
  else
    type = "** unknown mode **";
  printf("[file]:%-20.19s [type]:%18s [value]:%#8o\n", p, type,
         (buf.st_mode & S_IFMT));
}

bool is_setuid(char *p) {
  struct stat buf;
  if (lstat(p, &buf) < 0) {
    my_perror("lstat");
    return -1;
  }
  printf("st_mode:%#o\n", buf.st_mode); // 0100644
  return (buf.st_mode & S_ISUID) != 0;  // S_ISUID = 0004000
}

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    get_ft(argv[i]);
  }

// NOTE:
// On macOS (and modern BSDs), privilege separation is used, command like
// `passwd` doesn't rely on SUID being set, but a separated daemon for
// performing limited sensitive operations.
#ifdef LINUX
  assert(true == is_setuid("/usr/bin/passwd"));
#endif
  return 0;
}

/* Example:
- on macOS:
Debug/filedir/test_stat /etc/fstab /etc/ /dev/tty /dev/disk0 /dev/stdin
/var/run/usbmuxd tmp/data/filedir/fifo [file]:/var/log/system.log  [type]:
regular [value]: 0100000 [file]:/etc/                [type]:         directory
[value]:  040000 [file]:/dev/tty             [type]: character special [value]:
020000 [file]:/dev/disk0           [type]:     block special [value]:  060000
[file]:/dev/stdin           [type]:     symbolic link [value]: 0120000
[file]:/var/run/usbmuxd     [type]:            socket [value]: 0140000
[file]:tmp/data/filedir/fi  [type]:              fifo [value]:  010000

- on Linux:
Debug/filedir/test_stat /etc/fstab /etc/ /dev/tty /dev/loop0 /dev/stdin
/var/run/systemd/fsck.progress tmp/data/filedir/fifo [file]:/etc/fstab [type]:
regular [value]: 0100000 [file]:/etc/                [type]:         directory
[value]:  040000 [file]:/dev/tty             [type]: character special [value]:
020000 [file]:/dev/loop0           [type]:     block special [value]:  060000
[file]:/dev/stdin           [type]:     symbolic link [value]: 0120000
[file]:/var/run/systemd/fs  [type]:            socket [value]: 0140000
[file]:tmp/data/filedir/fi  [type]:              fifo [value]:  010000
 */
