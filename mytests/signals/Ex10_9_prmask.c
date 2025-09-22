#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/_types/_sigset_t.h>
#include <sys/signal.h>

#include "rltapue.h"

void pr_mask2_Ex10_9(const char *str, sigset_t mask) {
  printf("%s: ", str);
  while (mask > 0) {
    sigset_t lsb = -mask & mask; // find the least significant bit (the rightmost 1)
    int signo = __builtin_ctzl(lsb) + 1; // GCC/Clang builtin: count trailing zeros
    printf("|%s|, ", strsignal(signo));
    sigdelset(&mask, signo);
  }
  printf("\n");
}

void pr_mask_Ex10_9(const char *str) {
  sigset_t mask;
  if (sigprocmask(0, NULL, &mask) < 0) {
    my_perror("error: sigprocmask");
  } else {
    pr_mask2_Ex10_9(str, mask);
  }
}

void test_pr_mask() {
  sigset_t mask, old;
  sigfillset(&mask);
  sigemptyset(&old);
  sigprocmask(SIG_SETMASK, &mask, &old);
  pr_mask("pr_mask:");
  sigprocmask(SIG_SETMASK, &old, NULL);
}

void Ex10_9_pr_mask_test1() {
  sigset_t mask, old;
  sigfillset(&mask);
  sigemptyset(&old);
  sigprocmask(SIG_SETMASK, &mask, &old);
  pr_mask_Ex10_9("pr_mask");
  sigprocmask(SIG_SETMASK, &old, NULL);
}

void Ex10_9_pr_mask_test2() {
  sigset_t mask, old;
  sigemptyset(&mask);
  sigemptyset(&old);

  int signals[] = {SIGUSR1, SIGUSR2, SIGABRT, SIGCONT, SIGTSTP, SIGSTOP};
  for (int i = 0; i < sizeof(signals) / sizeof(int); i++) {
    sigaddset(&mask, signals[i]);
  }
  sigprocmask(SIG_SETMASK, &mask, &old);
  pr_mask_Ex10_9("pr_mask");
  sigprocmask(SIG_SETMASK, &old, NULL);
}

void Ex10_9_pr_mask_test3()
{
  sigset_t mask, old;
  sigemptyset(&mask);
  sigemptyset(&old);
  int signals[] = {SIGVTALRM, SIGPROF, SIGXCPU, SIGTTIN, SIGTTOU, SIGWINCH};
  for (int i = 0; i < sizeof(signals) / sizeof(int); i++) {
    sigaddset(&mask, signals[i]);
  }
  sigprocmask(SIG_SETMASK, &mask, &old);
  pr_mask("Current blocked signals: ");
  sigprocmask(SIG_SETMASK, &old, NULL);
}

int main(int argc, char *argv[]) {
  test_pr_mask();
  Ex10_9_pr_mask_test1();
  Ex10_9_pr_mask_test2();
  Ex10_9_pr_mask_test3();
  return 0;
}

/*

:!Debug/signals/Ex10_9_prmask
[No write since last change]
pr_mask:: |Hangup: 1|, |Interrupt: 2|, |Quit: 3|, |Illegal instruction: 4|, |Trace/BPT trap: 5|, |Abort trap: 6|, |EMT trap: 7|, |Floating point exception: 8|, |Bus error: 10|, |Segmentation fault: 11|,
|Bad system call: 12|, |Broken pipe: 13|, |Alarm clock: 14|, |Terminated: 15|, |Urgent I/O condition: 16|, |Suspended: 18|, |Continued: 19|, |Child exited: 20|, |Stopped (tty input): 21|, |Stopped (tty o
utput): 22|, |I/O possible: 23|, |Cputime limit exceeded: 24|, |Filesize limit exceeded: 25|, |Virtual timer expired: 26|, |Profiling timer expired: 27|, |Window size changes: 28|, |Information request:
29|, |User defined signal 1: 30|, |User defined signal 2: 31|, |Unknown signal: 32|,
pr_mask: |Hangup: 1|, |Interrupt: 2|, |Quit: 3|, |Illegal instruction: 4|, |Trace/BPT trap: 5|, |Abort trap: 6|, |EMT trap: 7|, |Floating point exception: 8|, |Bus error: 10|, |Segmentation fault: 11|, |
Bad system call: 12|, |Broken pipe: 13|, |Alarm clock: 14|, |Terminated: 15|, |Urgent I/O condition: 16|, |Suspended: 18|, |Continued: 19|, |Child exited: 20|, |Stopped (tty input): 21|, |Stopped (tty ou
tput): 22|, |I/O possible: 23|, |Cputime limit exceeded: 24|, |Filesize limit exceeded: 25|, |Virtual timer expired: 26|, |Profiling timer expired: 27|, |Window size changes: 28|, |Information request: 2
9|, |User defined signal 1: 30|, |User defined signal 2: 31|, |Unknown signal: 32|,
pr_mask: |Abort trap: 6|, |Suspended: 18|, |Continued: 19|, |User defined signal 1: 30|, |User defined signal 2: 31|,
Current blocked signals: : |Stopped (tty input): 21|, |Stopped (tty output): 22|, |Cputime limit exceeded: 24|, |Virtual timer expired: 26|, |Profiling timer expired: 27|, |Window size changes: 28|,

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
