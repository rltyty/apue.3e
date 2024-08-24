#include <stdio.h>
#include <unistd.h>

void test_fd1()
{
  printf("fd=%d\n", 1);
  dup2(1, 0);
  dup2(1, 1);
  dup2(1, 2);
  close(1);
}

int main(void)
{
  test_fd1();
}
