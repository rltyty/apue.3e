#include <signal.h>
#include <stdio.h>
#include <string.h>

void print_signal_info(int sig) {
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || \
    defined(__OpenBSD__)
  if (sig > 0 && sig < NSIG) {
    printf("sys_signame[%d]=%s, sys_siglist[%d]=%s, strsignal[%d]=%s\n", sig,
           sys_signame[sig], sig, sys_siglist[sig], sig, strsignal(sig));
  }
#else
  printf("strsignal(%d): %s\n", sig, strsignal(sig));
#endif
}

int main(int argc, char *argv[]) {
  for (int i = 0; i < NSIG; i++) {
    print_signal_info(i);
  }
  psignal(SIGSTOP, "STOP");
  psignal(SIGTSTP, "TSTP");
  return 0;
}

/*

On macOS:
:!Debug/signals/signamenum
STOP: Suspended (signal)                      <-- stderr
TSTP: Suspended                               <-- stderr
sys_signame[1]=hup, sys_siglist[1]=Hangup, strsignal[1]=Hangup: 1
sys_signame[2]=int, sys_siglist[2]=Interrupt, strsignal[2]=Interrupt: 2
sys_signame[3]=quit, sys_siglist[3]=Quit, strsignal[3]=Quit: 3
sys_signame[4]=ill, sys_siglist[4]=Illegal instruction, strsignal[4]=Illegal
instruction: 4 sys_signame[5]=trap, sys_siglist[5]=Trace/BPT trap,
strsignal[5]=Trace/BPT trap: 5 sys_signame[6]=abrt, sys_siglist[6]=Abort trap,
strsignal[6]=Abort trap: 6 sys_signame[7]=emt, sys_siglist[7]=EMT trap,
strsignal[7]=EMT trap: 7 sys_signame[8]=fpe, sys_siglist[8]=Floating point
exception, strsignal[8]=Floating point exception: 8 sys_signame[9]=kill,
sys_siglist[9]=Killed, strsignal[9]=Killed: 9 sys_signame[10]=bus,
sys_siglist[10]=Bus error, strsignal[10]=Bus error: 10 sys_signame[11]=segv,
sys_siglist[11]=Segmentation fault, strsignal[11]=Segmentation fault: 11
sys_signame[12]=sys, sys_siglist[12]=Bad system call, strsignal[12]=Bad system
call: 12 sys_signame[13]=pipe, sys_siglist[13]=Broken pipe, strsignal[13]=Broken
pipe: 13 sys_signame[14]=alrm, sys_siglist[14]=Alarm clock, strsignal[14]=Alarm
clock: 14 sys_signame[15]=term, sys_siglist[15]=Terminated,
strsignal[15]=Terminated: 15 sys_signame[16]=urg, sys_siglist[16]=Urgent I/O
condition, strsignal[16]=Urgent I/O condition: 16 sys_signame[17]=stop,
sys_siglist[17]=Suspended (signal), strsignal[17]=Suspended (signal): 17
sys_signame[18]=tstp, sys_siglist[18]=Suspended, strsignal[18]=Suspended: 18
sys_signame[19]=cont, sys_siglist[19]=Continued, strsignal[19]=Continued: 19
sys_signame[20]=chld, sys_siglist[20]=Child exited, strsignal[20]=Child exited:
20 sys_signame[21]=ttin, sys_siglist[21]=Stopped (tty input),
strsignal[21]=Stopped (tty input): 21 sys_signame[22]=ttou,
sys_siglist[22]=Stopped (tty output), strsignal[22]=Stopped (tty output): 22
sys_signame[23]=io, sys_siglist[23]=I/O possible, strsignal[23]=I/O possible: 23
sys_signame[24]=xcpu, sys_siglist[24]=Cputime limit exceeded,
strsignal[24]=Cputime limit exceeded: 24 sys_signame[25]=xfsz,
sys_siglist[25]=Filesize limit exceeded, strsignal[25]=Filesize limit exceeded:
25 sys_signame[26]=vtalrm, sys_siglist[26]=Virtual timer expired,
strsignal[26]=Virtual timer expired: 26 sys_signame[27]=prof,
sys_siglist[27]=Profiling timer expired, strsignal[27]=Profiling timer expired:
27 sys_signame[28]=winch, sys_siglist[28]=Window size changes,
strsignal[28]=Window size changes: 28 sys_signame[29]=info,
sys_siglist[29]=Information request, strsignal[29]=Information request: 29
sys_signame[30]=usr1, sys_siglist[30]=User defined signal 1, strsignal[30]=User
defined signal 1: 30 sys_signame[31]=usr2, sys_siglist[31]=User defined signal
2, strsignal[31]=User defined signal 2: 31

On Linux (Debian 12):
> ./Debug/signals/signamenum
STOP: Stopped (signal)                    <-- stderr
TSTP: Stopped                             <-- stderr
strsignal(0): Unknown signal 0
strsignal(1): Hangup
strsignal(2): Interrupt
strsignal(3): Quit
strsignal(4): Illegal instruction
strsignal(5): Trace/breakpoint trap
strsignal(6): Aborted
strsignal(7): Bus error
strsignal(8): Floating point exception
strsignal(9): Killed
strsignal(10): User defined signal 1
strsignal(11): Segmentation fault
strsignal(12): User defined signal 2
strsignal(13): Broken pipe
strsignal(14): Alarm clock
strsignal(15): Terminated
strsignal(16): Stack fault
strsignal(17): Child exited
strsignal(18): Continued
strsignal(19): Stopped (signal)
strsignal(20): Stopped
strsignal(21): Stopped (tty input)
strsignal(22): Stopped (tty output)
strsignal(23): Urgent I/O condition
strsignal(24): CPU time limit exceeded
strsignal(25): File size limit exceeded
strsignal(26): Virtual timer expired
strsignal(27): Profiling timer expired
strsignal(28): Window changed
strsignal(29): I/O possible
strsignal(30): Power failure
strsignal(31): Bad system call
strsignal(32): Unknown signal 32
strsignal(33): Unknown signal 33
strsignal(34): Real-time signal 0
strsignal(35): Real-time signal 1
strsignal(36): Real-time signal 2
strsignal(37): Real-time signal 3
strsignal(38): Real-time signal 4
strsignal(39): Real-time signal 5
strsignal(40): Real-time signal 6
strsignal(41): Real-time signal 7
strsignal(42): Real-time signal 8
strsignal(43): Real-time signal 9
strsignal(44): Real-time signal 10
strsignal(45): Real-time signal 11
strsignal(46): Real-time signal 12
strsignal(47): Real-time signal 13
strsignal(48): Real-time signal 14
strsignal(49): Real-time signal 15
strsignal(50): Real-time signal 16
strsignal(51): Real-time signal 17
strsignal(52): Real-time signal 18
strsignal(53): Real-time signal 19
strsignal(54): Real-time signal 20
strsignal(55): Real-time signal 21
strsignal(56): Real-time signal 22
strsignal(57): Real-time signal 23
strsignal(58): Real-time signal 24
strsignal(59): Real-time signal 25
strsignal(60): Real-time signal 26
strsignal(61): Real-time signal 27
strsignal(62): Real-time signal 28
strsignal(63): Real-time signal 29
strsignal(64): Real-time signal 30
 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
