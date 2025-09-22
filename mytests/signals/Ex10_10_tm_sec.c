#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

void test() {
  int loop = 0;
  struct timeval t;
  while (1) {
    loop++;
    if (loop == 5) {
      loop = 0;
      gettimeofday(&t, NULL);
      printf("%ld\n", t.tv_sec);
      fflush(stdout);
    }
    sleep(60);
  }
}

int main(int argc, char *argv[]) {
  test();
  return 0;
}

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
