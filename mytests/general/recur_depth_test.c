#include "rltapue.h"
#include <stdio.h>
#include <stdlib.h>

void recur(int depth)
{
  if (depth == 0) return;
  recur(depth - 1);
}

int main(int argc, char *argv[])
{
  if (argc > 1) {
    int d = atoi(argv[1]);
    recur(d);
    printf("Recur deep into %d levels. Done.\n", d);
  } else {
    my_perror("a.out <call depth>");
  }
}
