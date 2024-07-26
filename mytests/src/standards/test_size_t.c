#include <stddef.h>
#include <stdio.h>

int main(void) {
  size_t i = sizeof(int);
  printf("sizeof(int)=%ld\n", i);
  size_t l = sizeof(long);
  printf("sizeof(long)=%ld\n", l);
  size_t s = sizeof(size_t);
  printf("sizeof(size_t)=%ld\n", s);
  return 0;
}

/* sizeof(int)=4 */
/* sizeof(long)=8 */
/* sizeof(size_t)=8 */
