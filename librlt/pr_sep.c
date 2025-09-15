#include <stdio.h>

void pr_sep(char c, int count)
{
  for (int i = 0; i < count; i++) putchar(c);
  puts("");
}

void pr_sep_msg(char c, int count, char *msg)
{
  for (int i = 0; i < count; i++) putchar(c);
  putchar(' '); putchar(' ');
  fputs(msg, stdout);
  putchar(' '); putchar(' ');
  for (int i = 0; i < count; i++) putchar(c);
  puts("");
}

#ifdef test_pr_sep
#undef test_pr_sep
void test_pr_sep(void) {}
void test_pr_sep_msg(void) {}
int main(void) {
  test_pr_sep();
  test_pr_sep_msg();
  return 0;
}
#endif
