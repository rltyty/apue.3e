#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include "rltapue.h"

/**
- In a shell terminal, a session is created by a process calling `setsid()`
  which creates a new process group, the calling process becomes the group
  leader and the session leader (`SID=PID`, `PGID=PID`). The call detaches
  the process from the controlling terminal (if any).
  - Daemons typically call `setsid(2)` to detach from the terminal so they
    don’t get killed when you log out or press Ctrl-C.
- A session leader cannot leave (abandon) its session.
- The relationship between a process and a session is formed:
  - at process creation (`fork(2)`) via parent inheritance of SID and PGID
  - at session creation (`setsid(2)`) when the process becomes the session
    leader.
- The only way for a process to leave its current session is to create a new
  session and becomes its leader (via `setsid(2)`).

- A child process can have a different session ID from its parent's when:
  - The original parent creates a new session for itself (calls `setsid()`).
  - The child itself calls `setsid()` to create a new session.
  - When a parent exits, its orphaned children are reparented to `init`
    (PID 1), but their session ID does **not** change.

 */

int main(int argc, char *argv[]) {
  pid_t pid;

  pid_t sid = getsid(0); // get current session id
  printf("current session id: %d\n", sid);

  if ((pid = fork()) < 0) {
    my_perror("error: fork");
  } else if (pid == 0) { // child
    assert(getsid(0) == sid);
    printf("I'm child %d, my parent %d, my PGID %d, my SID %d\n",
           getpid(), getppid(), getpgid(0), getsid(0));

    printf("I'm child, I'm gonna create a grandchild for my parent.\n");
    if ((pid = fork()) < 0) {
      my_perror("error: fork grandchild");
    } else if (pid == 0) { // grandchild
      printf("I'm grandchild %d, my parent %d, my PGID %d, my SID %d\n",
             getpid(), getppid(), getpgid(0), getsid(0));
      sleep(3);
      printf("I'm grandchild %d, my parent %d, my PGID %d, my SID %d\n\n",
             getpid(), getppid(), getpgid(0), getsid(0));
      return 0;
    }
    sleep(1);
    printf("I'm child, I'm gonna create a new session for myself\n");
    sid = setsid();
    assert(getsid(0) == sid);
    printf("I'm child %d, my parent %d, my PGID %d, my SID %d\n",
           getpid(), getppid(), getpgid(0), getsid(0));
    sleep(5); // wait for parent exiting.
    printf("I'm child %d, my parent %d, my parent's SID %d, my PGID %d, my SID %d\n\n",
           getpid(), getppid(), getsid(getppid()), getpgid(0), getsid(0));
    return 0;
  } else { // parent
    assert(getsid(0) == sid);
    sleep(2);
    printf("I'm parent %d, my PGID %d, my SID %d\n",
           getpid(), getpgid(0), getsid(0));
  }

  return 0;
}

/*
> ./Debug/procgrp/sessid
current session id: 49433
I'm child 2716, my parent 2715, my PGID 2715, my SID 49433
I'm child, I'm gonna create a grandchild for my parent.
I'm grandchild 2717, my parent 2716, my PGID 2715, my SID 49433
I'm child, I'm gonna create a new session for myself
I'm child 2716, my parent 2715, my PGID 2716, my SID 2716
I'm parent 2715, my PGID 2715, my SID 49433
> I'm grandchild 2717, my parent 2716, my PGID 2715, my SID 49433

I'm child 2716, my parent 1, my parent's SID 1, my PGID 2716, my SID 2716
>

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
