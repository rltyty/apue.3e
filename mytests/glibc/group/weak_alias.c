#include <assert.h>

/**

sleep.c:
weak_alias (__sleep, sleep)

libc-symbols.h:
# define weak_alias(name, aliasname) _weak_alias (name, aliasname)
# define _weak_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((weak, alias (#name))) \
    __attribute_copy__ (name);

 */

static void lshift(int *p, int shift) {
  (*p) = (*p) << shift;
}

static void rshift(int *p, int shift) {
  (*p) = (*p) >> shift;
}

/**
 * 1. GNU C extension: __typeof(x) gives you the type of x
 * **`__typeof`** is a compiler keyword, not a macro nor a function. It just
 * gives the type of the declared symbol
 * 
 * `const`, `volatile`, `restrict`: they're port of type
 * `static`, `extern`, `auto`, `register`, `inline`: they're NOT part of type,
 * they're storage class or linkage.
 */

static __typeof(lshift) *shift;

void __typeof_test(void)
{
  int a;
  __typeof(a) b;    // int b;
  assert(sizeof(b) == 4);
  a = 5;
  lshift(&a, 2);
  assert(20 == a);
  b = 20;
  rshift(&b, 2);
  assert(5 == b);

  shift = lshift;
  shift(&b, 2);
  assert(20 == b);

  shift = rshift;
  shift(&a, 2);
  assert(5 == a);
}


/**
 * 2. __attribute__((weak, alias("name"))) tells the compiler/linker:
 *    - `aliasname` is just another name (an alias) for `name`
 *    - weak means:
 *      - If `aliasname` is referenced, but the real definition of `aliasname`
 *      exists elsewhere, that one overrides this weak alias.
 *      - Otherwise, `aliasname` just points to `name`
 */
#ifdef LINUX
__typeof(rshift) shift2 __attribute((weak, alias ("rshift")));
__typeof(lshift) shift3 __attribute((weak, alias ("lshift")));

/* strong alias, results in linker error: multiple definition of shift3 */
// __typeof(lshift) shift3 __attribute((alias ("lshift"))); // linker error
#elif defined(MACOS)
static __typeof(rshift) *shift2 = rshift;
static __typeof(lshift) *shift3 = lshift;
#endif

void __attribute___test(void)
{
  int a = 5;
  shift2(&a, 2);
  assert(1 == a);
  int b = 64;
  shift2(&b, 5);
  assert(2 == b);

  shift3(&b, 4);
#ifdef LINUX
  /* strong definition in shift3.c overrides weak alias */
  assert(64 == b);
#elif defined(MACOS)
  assert(32 == b);
#endif
}

#ifdef LINUX
/**
 * 3. __attribute_copy__(name) a glibc extension: it copies attributes (like
 * nonnull, malloc, format(printf, …) etc.) from name to aliasname.
 *
 */

__typeof(lshift) shift4 __attribute((weak, alias ("rshift"))) \
    __attribute_copy__(lshift);

void __attribute_copy___test(void)
{
  int a = 64;
  shift4(&a, 5);
  assert(2 == a);
}
#endif

int main(int argc, char *argv[]) {

  __typeof_test();
  __attribute___test();
#ifdef LINUX
  __attribute_copy___test();
#endif

  return 0;
}

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
