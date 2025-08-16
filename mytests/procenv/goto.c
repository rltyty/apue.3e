#include <stdio.h>

int main(int argc, char *argv[])
{
  int a = 0;
  int b = 1;
  int c = 2;
  printf("a = %d\n", a);

  goto printc;
  printf("b = %d\n", b);


printc:
  printf("c = %d\n", c);
  return 0;
}

/**
!Debug/procenv/goto
a = 0
c = 2
*/
