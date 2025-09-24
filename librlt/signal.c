#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "rltapue.h"

void pr_statsig(int status) {
  if (WIFSTOPPED(status)) {
    printf("Child stopped by signal %d (%s)\n", WSTOPSIG(status),
           strsignal(WSTOPSIG(status)));
  } else if (WIFEXITED(status)) {
    printf("Child exited with status %d\n", WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    printf("Child killed by signal %d (%s)\n", WTERMSIG(status),
           strsignal(WTERMSIG(status)));
  } else if (WIFCONTINUED(status)) {
    printf("Child continued by signal %d (%s)\n", SIGCONT, strsignal(SIGCONT));
  } else {
    printf("Unknown status %d \n", status);
  }
}

char *pidstr(pid_t pid) {
  char *s = malloc(8);
  if (!s)
    return NULL;
  snprintf(s, 8, "%d", pid);
  return s;
}

void ps(pid_t pid) {
  char fmt[100] = "ps -o uid,pid,ppid,pgid,sess,tty,state,comm -p %d";
  char *cmd = malloc(128);
  if (sprintf(cmd, fmt, pid) < 0) {
    my_perror("error: sprintf");
  }
  system(cmd);
}

void pr_mask(const char *str) {
  sigset_t set;
  int errno_save = errno; /* we can be called by signal handlers */
  if (sigprocmask(0, NULL, &set) < 0) {
    err_ret("sigprocmask error");
  } else {
    pr_mask3(str, set);
  }
  errno = errno_save; /* restore errno */
}

void pr_mask2(const char *str, sigset_t *mask) {
  printf("%s", str);
  if (sigismember(mask, SIGINT))
    printf(" SIGINT");
  if (sigismember(mask, SIGQUIT))
    printf(" SIGQUIT");
  if (sigismember(mask, SIGUSR1))
    printf(" SIGUSR1");
  if (sigismember(mask, SIGUSR2))
    printf(" SIGUSR2");
  if (sigismember(mask, SIGALRM))
    printf(" SIGALRM");
  if (sigismember(mask, SIGABRT))
    printf(" SIGABRT");
  if (sigismember(mask, SIGTSTP))
    printf(" SIGTSTP");
  if (sigismember(mask, SIGCHLD))
    printf(" SIGCHLD");

  // this two are never blocked
  if (sigismember(mask, SIGKILL))
    printf(" SIGKILL");
  if (sigismember(mask, SIGSTOP))
    printf(" SIGSTOP");

  /* remaining signals can go here  */

  printf("\n");
}

void pr_mask3(const char *str, sigset_t mask) {
  printf("%s: ", str);

#ifdef __APPLE__
  while (mask > 0) {
    sigset_t lsb = -mask & mask; // find least significant bit (the rightmost 1)
    int signo = __builtin_ctzl(lsb) + 1; // use GCC/Clang count trailing zeros
    printf("|%d:%s|, ", signo, strsignal(signo));
    mask &= mask - 1 // clear the lsb
  }
#elif __linux__
  printf("\n__val[0] = %lx\n", mask.__val[0]);
  unsigned long int m = mask.__val[0];
  while (m > 0) {
    unsigned long int lsb = -m & m;             // get the lsb
    int signo = __builtin_ctzl(lsb) + 1;
    printf("|%d:%s|, ", signo, strsignal(signo));
    m &= m - 1;                                 // clear the lsb
  }
#else
#endif
  printf("\n");
}

#ifdef test_signal
#undef test_signal
int main(void) {
  sigset_t mask, new, old;
  sigemptyset(&mask);
  sigemptyset(&new);
  sigemptyset(&old);

  /* Test 1: Block 6 signals */
  int signals[] = {SIGUSR1, SIGUSR2, SIGABRT, SIGCONT, SIGKILL, SIGSTOP};
  for (int i = 0; i < sizeof(signals) / sizeof(int); i++) {
    sigaddset(&mask, signals[i]);
  }
  pr_mask3("Set these signals blocked", mask);
  sigprocmask(SIG_SETMASK, &mask, &old);
  pr_mask("Current blocked signals");
  sigprocmask(SIG_SETMASK, &old, NULL);

  pr_sep('-', 60);

  /* Test 2: Block 6 signals */
  sigfillset(&mask);
  pr_mask3("Set all signals blocked", mask);
  sigprocmask(SIG_SETMASK, &mask, &old);
  pr_mask("Current blocked signals");
  sigprocmask(SIG_SETMASK, &old, NULL);
  return 0;
}
#endif

/*
 *
On Debian 12 Linux:

 1) HUP      2) INT      3) QUIT     4) ILL      5) TRAP     6) IOT
 7) BUS      8) FPE      9) KILL    10) USR1    11) SEGV    12) USR2
13) PIPE    14) ALRM    15) TERM    16) STKFLT  17) CHLD    18) CONT
19) STOP    20) TSTP    21) TTIN    22) TTOU    23) URG     24) XCPU
25) XFSZ    26) VTALRM  27) PROF    28) WINCH   29) POLL    30) PWR
31) SYS     %

> ./librlt/signal
Set these signals blocked:
__val[0] = 60b20
|6:Aborted|, |9:Killed|, |10:User defined signal 1|, |12:User defined signal 2|, |18:Continued|, |19:Stopped (signal)|,
Current blocked signals:
__val[0] = 20a20                  <-- sigprocmask() won't block 9:KILL, 19:STOP
|6:Aborted|, |10:User defined signal 1|, |12:User defined signal 2|, |18:Continued|,
------------------------------------------------------------
Set all signals blocked:
__val[0] = fffffffe7fffffff       <-- sigsetfill() clear 32, 33
|1:Hangup|, |2:Interrupt|, |3:Quit|, |4:Illegal instruction|, |5:Trace/breakpoint trap|, |6:Aborted|, |7:Bus error|, |8:Floating point exception|, |9:Killed|, |10:User defined signal 1|, |11:Segmentation fault|, |12:User defined signal 2|, |13:Broken pipe|, |14:Alarm clock|, |15:Terminated|, |16:Stack fault|, |17:Child exited|, |18:Continued|, |19:Stopped (signal)|, |20:Stopped|, |21:Stopped (tty input)|, |22:Stopped (tty output)|, |23:Urgent I/O condition|, |24:CPU time limit exceeded|, |25:File size limit exceeded|, |26:Virtual timer expired|, |27:Profiling timer expired|, |28:Window changed|, |29:I/O possible|, |30:Power failure|, |31:Bad system call|, |34:Real-time signal 0|, |35:Real-time signal 1|, |36:Real-time signal 2|, |37:Real-time signal 3|, |38:Real-time signal 4|, |39:Real-time signal 5|, |40:Real-time signal 6|, |41:Real-time signal 7|, |42:Real-time signal 8|, |43:Real-time signal 9|, |44:Real-time signal 10|, |45:Real-time signal 11|, |46:Real-time signal 12|, |47:Real-time signal 13|, |48:Real-time signal 14|, |49:Real-time signal 15|, |50:Real-time signal 16|, |51:Real-time signal 17|, |52:Real-time signal 18|, |53:Real-time signal 19|, |54:Real-time signal 20|, |55:Real-time signal 21|, |56:Real-time signal 22|, |57:Real-time signal 23|, |58:Real-time signal 24|, |59:Real-time signal 25|, |60:Real-time signal 26|, |61:Real-time signal 27|, |62:Real-time signal 28|, |63:Real-time signal 29|, |64:Real-time signal 30|,
Current blocked signals:
__val[0] = fffffffe7ffbfeff       <-- sigprocmask() won't block 9:KILL, 19:STOP
|1:Hangup|, |2:Interrupt|, |3:Quit|, |4:Illegal instruction|, |5:Trace/breakpoint trap|, |6:Aborted|, |7:Bus error|, |8:Floating point exception|, |10:User defined signal 1|, |11:Segmentation fault|, |12:User defined signal 2|, |13:Broken pipe|, |14:Alarm clock|, |15:Terminated|, |16:Stack fault|, |17:Child exited|, |18:Continued|, |20:Stopped|, |21:Stopped (tty input)|, |22:Stopped (tty output)|, |23:Urgent I/O condition|, |24:CPU time limit exceeded|, |25:File size limit exceeded|, |26:Virtual timer expired|, |27:Profiling timer expired|, |28:Window changed|, |29:I/O possible|, |30:Power failure|, |31:Bad system call|, |34:Real-time signal 0|, |35:Real-time signal 1|, |36:Real-time signal 2|, |37:Real-time signal 3|, |38:Real-time signal 4|, |39:Real-time signal 5|, |40:Real-time signal 6|, |41:Real-time signal 7|, |42:Real-time signal 8|, |43:Real-time signal 9|, |44:Real-time signal 10|, |45:Real-time signal 11|, |46:Real-time signal 12|, |47:Real-time signal 13|, |48:Real-time signal 14|, |49:Real-time signal 15|, |50:Real-time signal 16|, |51:Real-time signal 17|, |52:Real-time signal 18|, |53:Real-time signal 19|, |54:Real-time signal 20|, |55:Real-time signal 21|, |56:Real-time signal 22|, |57:Real-time signal 23|, |58:Real-time signal 24|, |59:Real-time signal 25|, |60:Real-time signal 26|, |61:Real-time signal 27|, |62:Real-time signal 28|, |63:Real-time signal 29|, |64:Real-time signal 30|,

*/
