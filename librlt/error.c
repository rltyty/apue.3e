#include "rltapue.h"
#include <stdarg.h>
#include <stdio.h>

/* perror: message-formatting version */
void my_perror(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  char buf[MAXLINE];
  vsnprintf(buf, MAXLINE, fmt, ap);
  va_end(ap);
  perror(buf);
  fflush(NULL);
}

#ifdef test_error
void test_my_perror(void) {
  my_perror("Test from test_my_perror()");
  my_perror("My favourite football player: %s, football team: %s",
      "Lionel Messi", "Argentina");
}

int main(void) {
  test_my_perror();
  return 0;
}
#endif
