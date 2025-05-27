#include <assert.h>
#include <stdio.h>
#include <unistd.h>

void test_fd1()
{
  printf("fd=%d\n", 1);
  assert(0 == dup2(1, 0));
  assert(1 == dup2(1, 1));
  assert(2 == dup2(1, 2));
  close(1);
}

int main(void)
{
  test_fd1();
}
