#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rltapue.h"

int init_file(const char *const path) {
  if (!path) return -1;
  FILE *pf = fopen(path, "w");
  if (!pf) my_perror("error: fopen");
  truncate(path, 0);

  int a = 0;
  // Write the character '0' (ASCII 0x30) to the file
  if (fprintf(pf, "%d", a) < 0) {
    my_perror("error: fprintf() with ferror()=%d", ferror(pf));
  }
  fclose(pf);
  return 0;
}

int incre_file(FILE *pf) {
  int a;
  int n = fscanf(pf, "%d", &a);
  if (n == EOF) {
    if (feof(pf)) {
      printf("End of file reached.\n");
    } else if (ferror(pf)) {
      printf("Read error occurred (ferror set).\n");
    }
    my_perror("error: fscanf()");
  } else if (n == 1) {
    a++;
    rewind(pf);
    if (fprintf(pf, "%d", a) < 0) {
      my_perror("error: fprintf() with ferror()=%d", ferror(pf));
    }
  }
  return 0;
}

void test(void) {
  const char *const file = "tmp/data/signals/sharedfile";
  init_file(file);
  TELL_WAIT();
  pid_t pid = fork();
  if (pid < 0) {
    my_perror("error: fork");
  } else if (pid == 0) {  // child
    for (int i = 0; i < 3; i++) {
      WAIT_PARENT();
      // critical section
      FILE *pf = fopen(file, "r+");
      if (!pf) my_perror("error: fopen in rw mode");
      printf("Child[%d]: increase the counter\n", getpid());
      incre_file(pf);
      fclose(pf);
      TELL_PARENT(getppid());
    }
    exit(0);
  } else {  // parent
    for (int i = 0; i < 3; i++) {
      // critical section
      FILE *pf = fopen(file, "r+");
      if (!pf) my_perror("error: fopen in rw mode");
      printf("Parent[%d]: increase the counter\n", getpid());
      incre_file(pf);
      fclose(pf);
      TELL_CHILD(pid);
      WAIT_CHILD();
    }
    exit(0);
  }
}

int main(int argc, char **argv) {
  test();
  return 0;
}

/*

:!Debug/signals/Ex10_6_procsync
Parent[39051]: increase the counter
Parent[39051]: increase the counter
Parent[39051]: increase the counter
Child[39052]: increase the counter
Child[39052]: increase the counter
Child[39052]: increase the counter

:!cat ./tmp/data/signals/sharedfile
6
 */
