#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rltapue.h"

int main(int argc, char *argv[]) {
  printf("I'm printenv: PID=%d\n", getpid());
  for (int i = 1; i < argc; i++) {
    printf("ENV[%s] = %s\n", argv[i], getenv(argv[i]));
  }

  printf("Now print all environ.\n");
  pr_environ();
  return 0;
}
