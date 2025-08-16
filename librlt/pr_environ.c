#include <stdio.h>

void pr_environ()
{
  extern char **environ;
  for (char **p = environ; *p != NULL; p++) {
    printf("%s\n", *p);
  }
}
