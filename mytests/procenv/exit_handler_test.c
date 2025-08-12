#include <stdio.h>
#include <stdlib.h>
#include "rltapue.h"

static char *p1 = "Bye!";
static char *p2 = "Bye!";

void exit_handler_1(void);
void exit_handler_2(void);

int main(int argc, char *argv[]) {
  if (atexit(exit_handler_1)) my_perror("atexit: exit_handler_1");
  if (atexit(exit_handler_1)) my_perror("atexit: exit_handler_1");
  if (atexit(exit_handler_2)) my_perror("atexit: exit_handler_2");
  if (argc > 1) {
    p1 = argv[1];
    if (argc > 2) {
      p2 = argv[2];
    }
  }
  printf ("argc = %d\n", argc);
}

void exit_handler_1 (void) {
  printf("EXIT_HANDLER_1: %s\n", p1);
}

void exit_handler_2 (void) {
  printf("EXIT_HANDLER_2: %s\n", p2);
}

/*
:!Debug/procenv/exit_handler_test 再见 Adios
argc = 3
EXIT_HANDLER_2: Adios
EXIT_HANDLER_1: 再见
EXIT_HANDLER_1: 再见
 */
