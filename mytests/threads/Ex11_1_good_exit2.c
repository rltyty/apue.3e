/**
 * Compared with Figure 11.4, use dynamic memory allocation from heap
 * Option 1: allocate in main() and pass the address to thr_fn1()
 * Option 2: allocate in thr_fn1() and pass the address to pthread_exit()
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rltapue.h"

struct foo {
	int a, b, c, d;
};

void
printfoo(const char *s, const struct foo *fp)
{
	printf("%s", s);
	printf("  structure at 0x%lx\n", (unsigned long)fp);
	printf("  foo.a = %d\n", fp->a);
	printf("  foo.b = %d\n", fp->b);
	printf("  foo.c = %d\n", fp->c);
	printf("  foo.d = %d\n", fp->d);
}

void *
thr_fn1(void *arg)
{
        struct foo *fp = malloc(sizeof(struct foo));
        if (fp == NULL) {
          my_perror("error: malloc() for struct foo.");
        }
        fp->a = 1;
        fp->b = 2;
        fp->c = 3;
        fp->d = 4;
	printfoo("thread 1:\n", fp);
	pthread_exit((void*)fp);
}

void *
thr_fn2(void *arg)
{
	printf("thread 2: ID is %lu\n", (unsigned long)pthread_self());
	pthread_exit((void *)0);
}

int
main(void)
{
	int             err;
	pthread_t	tid1, tid2;
	struct foo	*fp;

	err = pthread_create(&tid1, NULL, thr_fn1, NULL);
	if (err != 0)
		err_exit(err, "can't create thread 1");
	err = pthread_join(tid1, (void **)&fp);
	if (err != 0)
		err_exit(err, "can't join with thread 1");
	sleep(1);
	printf("parent starting second thread\n");
	err = pthread_create(&tid2, NULL, thr_fn2, NULL);
	if (err != 0)
		err_exit(err, "can't create thread 2");
	sleep(1);
	printfoo("parent:\n", fp);
        free(fp);
	exit(0);
}

/*

:!Debug/threads/Ex11_1_good_exit2
thread 1:
  structure at 0x600000d38040
  foo.a = 1
  foo.b = 2
  foo.c = 3
  foo.d = 4
parent starting second thread
thread 2: ID is 123145375821824
parent:
  structure at 0x600000d38040
  foo.a = 1
  foo.b = 2
  foo.c = 3
  foo.d = 4


 */
// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
