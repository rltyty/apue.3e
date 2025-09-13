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
