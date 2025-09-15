#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/signal.h>
#include "apue.h"
#include "rltapue.h"

/**
 * Signal numberings on Linux (SysV-ish) and macOS (BSD-ish) are different.
 */

#define SIG2STR_MAX 32 // string length max

#define sigentry(signo) [signo] = #signo

#ifdef __APPLE__
#define signo_array(macro) \
  macro(SIGHUP),  macro(SIGINT),    macro(SIGQUIT), macro(SIGILL),    \
  macro(SIGTRAP), macro(SIGABRT),   macro(SIGEMT),  macro(SIGFPE),    \
  macro(SIGKILL), macro(SIGBUS),    macro(SIGSEGV), macro(SIGSYS),    \
  macro(SIGPIPE), macro(SIGALRM),   macro(SIGTERM), macro(SIGURG),    \
  macro(SIGSTOP), macro(SIGTSTP),   macro(SIGCONT), macro(SIGCHLD),   \
  macro(SIGTTIN), macro(SIGTTOU),   macro(SIGIO),   macro(SIGXCPU),   \
  macro(SIGXFSZ), macro(SIGVTALRM), macro(SIGPROF), macro(SIGWINCH),  \
  macro(SIGINFO), macro(SIGUSR1),   macro(SIGUSR2)
#elif defined(__linux__)
#define signo_array(macro) \
  macro(SIGHUP),  macro(SIGINT),    macro(SIGQUIT), macro(SIGILL),    \
  macro(SIGTRAP), macro(SIGABRT),   macro(SIGBUS),  macro(SIGFPE),    \
  macro(SIGKILL), macro(SIGUSR1),   macro(SIGSEGV), macro(SIGUSR2),   \
  macro(SIGPIPE), macro(SIGALRM),   macro(SIGTERM), macro(SIGSTKFLT), \
  macro(SIGCHLD), macro(SIGCONT),   macro(SIGSTOP), macro(SIGTSTP),   \
  macro(SIGTTIN), macro(SIGTTOU),   macro(SIGURG),  macro(SIGXCPU),   \
  macro(SIGXFSZ), macro(SIGVTALRM), macro(SIGPROF), macro(SIGWINCH),  \
  macro(SIGPOLL), macro(SIGPWR),    macro(SIGSYS)
#endif

const char* sig_names[] = {
  signo_array(sigentry)
};


int sig2str(int signo, char *str) {
  if (signo < 0 || signo > sizeof(sig_names) / sizeof(sig_names[0])) {
    return -1;
  }
  if (sig_names[signo] == NULL) {
    return -1;
  }
  strncpy(str, sig_names[signo]+3, SIG2STR_MAX-1);
  str[SIG2STR_MAX-1] = '\0';
  return 0;
}

int main(int argc, char *argv[]) {
  char name[SIG2STR_MAX];
  int sigcount = sizeof(sig_names) / sizeof(char*);
  for (int i = 0; i < sigcount; i++) {
    if (sig2str(i, name) == -1) {
      err_msg("error: sig2str(%d)", i);
    } else {
      printf("sig[%d]: %s\n", i, name);
    }
  }
  return 0;
}

/*

On macOS:

:!Debug/signals/Ex10_2_sig2str
error: sig2str(0)
sig[1]: HUP
sig[2]: INT
sig[3]: QUIT
sig[4]: ILL
sig[5]: TRAP
sig[6]: ABRT
sig[7]: EMT
sig[8]: FPE
sig[9]: KILL
sig[10]: BUS
sig[11]: SEGV
sig[12]: SYS
sig[13]: PIPE
sig[14]: ALRM
sig[15]: TERM
sig[16]: URG
sig[17]: STOP
sig[18]: TSTP
sig[19]: CONT
sig[20]: CHLD
sig[21]: TTIN
sig[22]: TTOU
sig[23]: IO
sig[24]: XCPU
sig[25]: XFSZ
sig[26]: VTALRM
sig[27]: PROF
sig[28]: WINCH
sig[29]: INFO
sig[30]: USR1
sig[31]: USR2

On Linux Debian 12:
:!Debug/signals/sigstr
error: sig2str(0)
sig[1]: HUP
sig[2]: INT
sig[3]: QUIT
sig[4]: ILL
sig[5]: TRAP
sig[6]: ABRT
sig[7]: BUS
sig[8]: FPE
sig[9]: KILL
sig[10]: USR1
sig[11]: SEGV
sig[12]: USR2
sig[13]: PIPE
sig[14]: ALRM
sig[15]: TERM
sig[16]: STKFLT
sig[17]: CHLD
sig[18]: CONT
sig[19]: STOP
sig[20]: TSTP
sig[21]: TTIN
sig[22]: TTOU
sig[23]: URG
sig[24]: XCPU
sig[25]: XFSZ
sig[26]: VTALRM
sig[27]: PROF
sig[28]: WINCH
sig[29]: POLL
sig[30]: PWR
sig[31]: SYS

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
