#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

// Helper macro to select the 8th argument (0-indexed).
#define _GET_NTH_ARG(_7,_6,_5,_4,_3,_2,_1,_0,n,...) n

#if defined(__STDC_VERSION__)
#if __STDC_VERSION__ >= 202311L
// C23-compliant macro to count arguments from 0 to 8.
// __VA_OPT__(,) adds a comma only if __VA_ARGS__ is not empty.
#define _COUNT_ARGS(...) _GET_NTH_ARG(__VA_ARGS__ __VA_OPT__(,) 8,7,6,5,4,3,2,1,0)
#else
#define _COUNT_ARGS(...) _GET_NTH_ARG(__VA_ARGS__,8,7,6,5,4,3,2,1,0)
#endif
#endif

void variadic_macro_argument_counting_test()
{
  printf("__STDC_VERSION__ = %ld\n", __STDC_VERSION__);
#if defined(__STDC_VERSION__)
#if __STDC_VERSION__ >= 202311L
  assert(0 == _COUNT_ARGS()); // This now works!
#endif
#endif
  assert(1 == _COUNT_ARGS('a'));
  assert(2 == _COUNT_ARGS('a', "b"));
  assert(3 == _COUNT_ARGS('a', "b", 3.0));
  assert(4 == _COUNT_ARGS('a', "b", 3.0, 4L));
  assert(5 == _COUNT_ARGS('a', "b", 3.0, 4L, true));
  assert(6 == _COUNT_ARGS('a', "b", 3.0, 4L, true, false));
  assert(7 == _COUNT_ARGS('a', "b", 3.0, 4L, true, false, "July"));
  // This macro is limited to 8 arguments.
  assert(8 == _COUNT_ARGS('a', "b", 3.0, 4L, true, false, "July", "August"));
}

int main() {
    variadic_macro_argument_counting_test();
    return 0;
}
