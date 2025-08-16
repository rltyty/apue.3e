#include <stdio.h>
#include <stdlib.h>
#include "rltapue.h"

int main(int argc, char *argv[])
{
  pr_sep('=', 70);

  pr_environ();

  pr_sep_msg('-', 30, "PATH");

  char *PATH = getenv("PATH");
  printf("PATH=%s\n", PATH);

  pr_sep_msg('-', 30, "Append a path to PATH");
  add_path("./Debug/procenv");

  PATH = getenv("PATH");
  printf("PATH=%s\n", PATH);
}
