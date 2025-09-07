#include <unistd.h>
int main(int argc, char *argv[]) {
  alarm(5); // modern version: setitimer(2)
  pause();  // modern version: sigsuspend(2)
            // race-condition-safe, precise control over signal masking
  return 0;
}

/*

> ./Debug/signals/sigalarm
[2]    16246 alarm      ./Debug/signals/sigalarm
> echo $?
142           <- (128 + SIGALRM(14))

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
