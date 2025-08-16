#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
  printf("I'm printargv: PID=%d\n", getpid());
  for (int i = 0; i < argc; i++) {
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  return 0;
}
