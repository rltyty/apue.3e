#include "rltapue.h"
#include <sys/wait.h>
#include <stdio.h>

void
pr_exit(int status)
{
	if (WIFEXITED(status))
		printf("normal termination, exit status = %d\n",
				WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		printf("abnormal termination, signal number = %d%s\n",
				WTERMSIG(status),
#ifdef	WCOREDUMP
				WCOREDUMP(status) ? " (core file generated)" : "");
#else
				"");
#endif
	else if (WIFSTOPPED(status))
		printf("child stopped, signal number = %d\n",
				WSTOPSIG(status));
}

#ifdef test_pr_exit
#undef test_pr_exit
void test_pr_exit(void) {}
int main(void) {
  test_pr_exit();
  return 0;
}
#endif
