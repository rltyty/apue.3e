#include <stdio.h>

void pr_sep(char c, int count)
{
  for (int i = 0; i < count; i++) printf("%c", c);
  printf("\n");
}

void pr_sep_msg(char c, int count, char *msg)
{
  for (int i = 0; i < count; i++) printf("%c", c);
  printf("  ");
  printf("%s", msg);
  printf("  ");
  for (int i = 0; i < count; i++) printf("%c", c);
  printf("\n");
}
