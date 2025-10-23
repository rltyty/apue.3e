#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

/*
 * Get the number of arguments for INLINE_SYSCALL_CALL(name, arg1, arg2, …)
 * __VA_ARGS__: name, arg1, arg2, ...
 *
 * Return value: 0 ~ 7. 0 means only function name, no args.
 */

#define _GET_NTH_ARG(_7,_6,_5,_4,_3,_2,_1,_0,n,...) n
#define _COUNT_ARGS(...) _GET_NTH_ARG(__VA_ARGS__,7,6,5,4,3,2,1,0)

void variadic_macro_argument_counting_test()
{
  assert(0 == _COUNT_ARGS("func"));
  assert(1 == _COUNT_ARGS("func", "a1"));
  assert(2 == _COUNT_ARGS("func", "a1", "a2"));
  assert(3 == _COUNT_ARGS("func", "a1", "a2", "a3"));
  assert(4 == _COUNT_ARGS("func", "a1", "a2", "a3", "a4"));
  assert(5 == _COUNT_ARGS("func", "a1", "a2", "a3", "a4", "a5"));
  assert(6 == _COUNT_ARGS("func", "a1", "a2", "a3", "a4", "a5", "a6"));
  assert(7 == _COUNT_ARGS("func", "a1", "a2", "a3", "a4", "a5", "a6", "a7"));

  // BTW, 
  assert(0 == _COUNT_ARGS());
}

#define sig2str(sig) #sig

void macro_num2str_test()
{
  assert(strcmp("SIGHUP", sig2str(SIGHUP)) == 0);
  assert(strcmp("SIGINT", sig2str(SIGINT)) == 0);
}

int main() {
    variadic_macro_argument_counting_test();
    macro_num2str_test();
    return 0;
}
