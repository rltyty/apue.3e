#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma clang diagnostic ignored "-Wsizeof-array-decay"
#pragma clang diagnostic ignored "-Wsizeof-array-argument"

// equivalent to void array_decay_int(int *a)
void array_decay_int(int a[]) {
  // a is decayed to a pointer (int *)
  assert(8 == sizeof(int *));
  assert(sizeof(int *) == sizeof(a));
}

void array_decay_char(char a[]) {
  assert(8 == sizeof(char *));
  assert(sizeof(char *) == sizeof(a));
}

// To eliminate above two warnings w/o preprocessor, use pointer argument
void array_ptr_int(int *a) { ; }
void array_ptr_char(char *a) { ; }

void test_char_arr(void) {
  char a[] = "12345\n";
  assert(sizeof(a) == 7);  // include hidden '\0' at the end of the string
  assert(strlen(a) == 6);  // exclude hidden '\0'
  printf("%s", a);

  char b[10] = "12345\n";   // partial initialization with padding zeros
  assert(sizeof(b) == 10);  // "12345\n0000"
  assert(strlen(b) == 6);
  printf("%s", b);
}

void test_string(void) { assert(7 == sizeof("12345\n")); }

int main(void) {
  test_char_arr();
  test_string();          // "string" is a '\0' terminated char sequence

  char arr[108];
  char *ptr = arr;
  assert(108 == sizeof(arr));
  assert(8 == sizeof(&arr[0]));
  assert(8 == sizeof(ptr));
  // array decayed to pointer when in expression and passed to function
  assert(8 == sizeof(arr + 1));  // NOLINT(bugprone-sizeof-expression)
  array_decay_char(arr);

  int a[108];
  assert(4 == sizeof(int));
  assert(432 == sizeof(a));

  // array decayed to pointer when in expression and passed to function
  assert(8 == sizeof(a + 1));  // NOLINT(bugprone-sizeof-expression)
  array_decay_int(a);

  // No direct method to get the size of memory allocated by malloc / calloc
  // from the pointer, but `free()` knows the size when collect the storage
  // due to the allocator allocates a bit larger than the requested size for
  // allocation metadata. The usable memory is just next to the metadata
  // that contains the size info.
  int *ap = (int *)malloc(sizeof(int) * 108);
  assert(8 == sizeof(ap));
  assert(8 == sizeof(ap + 1));
  int *bp = (int *)calloc(sizeof(int), 108);
  assert(8 == sizeof(bp));

  return 0;
}
