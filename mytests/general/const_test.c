#include <assert.h>

/**
 * The literals "hello" and "world" are immutable, no matter how to the
 * pointer is declared, they cannot be altered.
 * However, in `char p[] = "hello";` the literal is copied and used to
 * initialized a char array.
 */
void test_const(void)
{
  const char *p = "hello";  // the memory pointed to is const not the pointer
  assert(*(p+4) == 'o');
  // *(p+4) = 'a';          // ❌ Compile: Read-only variable is not assignable
  p = "world";              // point to another const memory
  assert(*(p+4) == 'd');
  // *(p+4) = 'a';          // ❌ Compile: Read-only variable is not assignable

  char *q = "hello";        // Backward compatible, no compilation error
  assert(*(q+4) == 'o');
  // *(q+4) = 'a';          // ❌ Runtime: Bus Error / Segmentation Fault

  char arr[] = "hello";
  assert(arr[4] == 'o');
  assert(*(arr+4) == 'o');
  *(arr+4) = 'a';
  assert(*(arr+4) == 'a');
}

int main(int argc, char *argv[]) {
  test_const();
  return 0;
}

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
