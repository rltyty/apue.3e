#include <stdio.h>

void pr_environ() {
  extern char **environ;
  for (char **p = environ; *p != NULL; p++) {
    printf("%s\n", *p);
  }
}

#ifdef test_pr_environ
#undef test_pr_environ
void test_pr_environ(void) {}
int main(void) {
  test_pr_environ();
  return 0;
}
#endif
