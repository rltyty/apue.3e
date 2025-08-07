/*
 * Write a utility like cp(1) that copies a file containing holes, without
 * writing the bytes of 0 to the output file.
 *
 * NOTE:
 * 1. "copies a file containing holes" explicitly means the holes should
 *    be preserved.
 * 2. "without writing the bytes of 0" implies copying holes as-is, rather
 *    than explicitly filling them with zeros.
 * 3. Actually, a 'hole' refers to an unwritten section - a vacant space
 *    filled with nothing. "Writing the bytes of 0" fills that area with
 *    zeros, allocates data blocks, increases disk usage, and eliminates
 *    the hole rather creating one.
 */

#include <sys/fcntl.h>

#include "rltapue.h"
int cp(const char *s, const char *t) {
  int sd = open(s, O_RDONLY);
  if (sd < 0) {
    my_perror("open %s", s);
  }
  int td = open(t, O_RDONLY);
  if (td < 0) {
    my_perror("open %s", t);
  }
  return 0;
}

int main(int argc, char *argv[]) { return 0; }
