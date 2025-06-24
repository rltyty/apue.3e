#include "rltapue.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("Usage: %s <file>\n", argv[0]);
  }
  struct stat stat;
  char *ftype;
  for (int i = 1; i < argc; i++) {
    if (lstat(argv[i], &stat) < 0) {
      my_perror("lstat error:");
    }
    get_ft(argv[i], &ftype);
    printf("file type: %s, file size: %lld\n", ftype, (long long)stat.st_size);
  }
  return 0;
}

/*
> Debug/filedir/Ex4_5 ../nvim.lua .nvim.lua ./tmp/data/filedir/fifo ./tmp/data/filedir/.gitkeep ./tmp/data/filedir/emptydir

file type: regular, file size: 320          [ ../nvim.lua ]
file type: symbolic link, file size: 11     [ .nvim.lua -> ../nvim.lua ]
file type: fifo, file size: 0
file type: regular, file size: 0            [ ./tmp/data/filedir/.gitkeep ]
file type: directory, file size: 64         [ 64 current dir size ]

> ls -al ./tmp/data/filedir/emptydir
total 0
drwxr-xr-x 2 gpanda staff  64 Jun 24 22:20 .    [ 64 current dir size ]
drwxr-xr-x 5 gpanda staff 160 Jun 24 22:20 ..

> ls -aR tmp/data/filedir
tmp/data/filedir:
.  ..  .gitkeep  emptydir  fifo             [ 5 entries = 32 * 5 = 160 ]

tmp/data/filedir/emptydir:                  [ 2 entries = 32 * 2 = 64 ]
.  ..

NOTE:
1. Looks like a directory entry is of 32 bytes size on macOS.
2. The file content size of a symbolic link is the target path string size.
e.g. `../nvim.lua`: 11 bytes
 */
