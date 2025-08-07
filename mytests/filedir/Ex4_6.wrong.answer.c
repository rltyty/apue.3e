#include "rltapue.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>

void cp(int, int);
void cp2(int, int, int);
void squeeze(char *, size_t, char);

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    my_perror("%s <src> <tgt>", argv[0]);
    return -1;
  }
  errno = 0;
  int fin, fout;
  if ((fin = open(argv[1], O_RDONLY)) < 0) {
    my_perror("open %s failed", argv[1]);
  }
  errno = 0;
  // if ((fout = creat(argv[2], FILE_MODE)) < 0) {
  if ((fout = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, FILE_MODE)) < 0) {
    my_perror("create %s failed", argv[2]);
  }

  cp2(fin, fout, 1);  // remove hole during copy

  if (close(fin) < 0) {
    my_perror("close");
  }
  if (close(fout) < 0) {
    my_perror("close");
  }
  return 0;
}

void cp2(int fsrc, int ftgt, int nohole) {
  char buf[BUFSIZ];
  size_t len;
  while ((len = read(fsrc, buf, BUFSIZ-1)) > 0) {
    buf[len] = '\0';
    if (nohole) {
      squeeze(buf, len, '\0');
      len = strlen(buf);
    }
    write(ftgt, buf, len);
  }
}

void cp(int fsrc, int ftgt) {
  cp2(fsrc, ftgt, 0);
}

void squeeze(char *buf, size_t size, char to_rm) {
  char *p = buf;
  char *q = buf;
  while (p < buf + size) {
    if (*p != to_rm) {
      *q++ = *p;
    }
    p++;
  }
  *q = '\0';
}

