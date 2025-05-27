#include "rltapue.h"
#include <stdio.h>

int main(void)
{
  printf("stdout\n");
  my_perror("stderr");
  return 0;
}

/*
1. By default, when a program is executed in a shell w/o explicit redirection,
both `stdout` and `stderr` are typically directed to the controlling terminal.

> Debug/fileio/Ex3_5_redirect
stderr: Undefined error: 0
stdout

2. Redirects stdout to xxx and stderr to yyy

> Debug/fileio/Ex3_5_redirect 1>xxx 2>yyy
> cat xxx
stdout
> cat yyy
stderr: Undefined error: 0

3. Redirects stdout to zzz and stderr to stdout, i.e. both to zzz

> Debug/fileio/Ex3_5_redirect >zzz 2>&1
> cat zzz
stderr: Undefined error: 0
stdout

4. Redirects stderr to stdout and stdout to zzz, i.e. stderr to the default
stdout (controlling terminal), stdout to zzz

> Debug/fileio/Ex3_5_redirect 2>&1 >zzz
stderr: Undefined error: 0
> cat zzz
stdout
 */
