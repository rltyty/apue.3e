#include <stdio.h>
#include <sys/resource.h>

void pr_rlimit(struct rlimit);

int main(int argc, char *argv[])
{
  struct rlimit lim;
#ifdef __APPLE__
  printf("RLIM_INFINITY = %lld\n", RLIM_INFINITY);
#elif defined(__linux__)
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
#ifdef __APPLE__
    printf("soft lim: %lld, ", lim.rlim_cur);
#elif defined(__linux__)
    printf("soft lim: %ld, ", lim.rlim_cur);
#endif
  }

  if (lim.rlim_max == RLIM_INFINITY) {
    printf("hard lim: unlimited, ");
  } else {
#ifdef __APPLE__
    printf("hard lim: %lld, ", lim.rlim_max);
#elif defined(__linux__)
    printf("hard lim: %ld, ", lim.rlim_max);
#endif
  }
}
