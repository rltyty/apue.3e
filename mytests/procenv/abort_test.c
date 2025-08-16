#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
  int d = 14;
  int m= 8;
  printf("d = %d\n", d);
  abort();
  printf("m = %d\n", m);

}

/*
./Debug/procenv/abort_test
d = 14

shell returned 134
 */
