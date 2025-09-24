#include <limits.h>
#include <signal.h>

#include "rltapue.h"

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

/*

On Debian 12 Linux:

➜  mytests git:(main) ✗ ./Debug/signals/Ex10_9_prmask
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

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
