#include <stdio.h>
#include <string.h>
#define STRSIZE 32

void strcpy_test(const char *src) {
  char dst[STRSIZE];
  strcpy(dst, src);
  puts(dst);
}

void strncpy_test(const char *src) {
  char dst[STRSIZE];
  strncpy(dst, src, STRSIZE-1);
  dst[STRSIZE-1] = '\0';
  puts(dst);
}

int main(int argc, char *argv[]) {

  strcpy_test("hello");
  strcpy_test(",");
  strncpy_test("world");
  strncpy_test("!");
  strcpy_test(NULL);     // SEGV: segmentation violation, when illegal memory
                         // access occurs, such as dereference a NULL pointer.
  return 0;
}

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
