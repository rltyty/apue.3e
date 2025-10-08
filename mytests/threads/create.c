#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/signal.h>
#include <unistd.h>

#include "rltapue.h"

void sig_intr(int signo) {
  printf("sig_intr: interrupted by %d: %s\n", signo, strsignal(signo));
}

typedef struct {
  int   num;
  char *name;
  int   sleep;

} Player;

void *thrd_func(void *arg) {
  Player *ppl = arg;
  printf("thread: <%2d %s>: sleep for %d secs\n",
         ppl->num, ppl->name, ppl->sleep);
  sleep(ppl->sleep);
  printf("thread: <%2d %s>: exits from thrd_func.\n", ppl->num, ppl->name);
  return NULL; // return ((void *) 0);
}

void create_test() {
  struct sigaction act = {0}, old = {0};
  act.sa_handler = sig_intr;
  sigaction(SIGINT, &act, &old);

  Player players[] = {
    {10,  "Lionel Messi",   20},
    {9,   "Julian Alvarez", 10},
  };

  for (int i = 0; i < sizeof(players) / sizeof(players[0]); i++) {
    pthread_t t;
    int error = pthread_create(&t, NULL, thrd_func, &players[i]);
    if (error != 0) {
      err_cont(error, "error: pthread_create");
    }
    printf("A new thread [0x%lx] was created in process [%d]\n",
           (unsigned long) t, getpid());
  }
  pause();
  sigaction(SIGINT, &old, NULL);
  printf("Main thread [0x%lx], bye!", (unsigned long) pthread_self());
}

int main(int argc, char *argv[]) {
  create_test();
  return 0;
}

/*

On macOS:

> ./Debug/threads/create
A new thread [0x7000036c2000] was created in process [67623]
A new thread [0x700003745000] was created in process [67623]
thread: < 9 Julian Alvarez>: sleep for 10 secs
thread: <10 Lionel Messi>: sleep for 20 secs
thread: < 9 Julian Alvarez>: exits from thrd_func.
thread: <10 Lionel Messi>: exits from thrd_func.
^Csig_intr: interrupted by 2: Interrupt: 2
Main thread [0x10c045600], bye!%

On Linux:

> ./Debug/threads/create
A new thread [0x7fc68df666c0] was created in process [1774883]
A new thread [0x7fc68d7656c0] was created in process [1774883]
thread: <10 Lionel Messi>: sleep for 20 secs
thread: < 9 Julian Alvarez>: sleep for 10 secs
thread: < 9 Julian Alvarez>: exits from thrd_func.
thread: <10 Lionel Messi>: exits from thrd_func.
^Csig_intr: interrupted by 2: Interrupt
Main thread [0x7fc68df67740], bye!%
 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
