#include <stdio.h>
#include <sys/resource.h>

void pr_rlimit(struct rlimit);

int main(int argc, char *argv[])
{
  struct rlimit lim;
#ifdef MACOS
  printf("RLIM_INFINITY = %lld\n", RLIM_INFINITY);
#elif defined(LINUX)
  printf("RLIM_INFINITY = %ld\n", RLIM_INFINITY);
#endif

  getrlimit(RLIMIT_DATA, &lim);
  pr_rlimit(lim);
  return 0;
}

void pr_rlimit(struct rlimit lim) {
  if (lim.rlim_cur == RLIM_INFINITY) {
    printf("soft lim: unlimited, ");
  } else {
    printf("soft lim: %ld, ", lim.rlim_cur);
  }

  if (lim.rlim_max == RLIM_INFINITY) {
    printf("hard lim: unlimited, ");
  } else {
#ifdef MACOS
    printf("hard lim: %lld, ", lim.rlim_max);
#elif defined(LINUX)
    printf("hard lim: %ld, ", lim.rlim_max);
#endif
  }
}
