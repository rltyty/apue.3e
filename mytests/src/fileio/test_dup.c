#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "apue.h"

void test_dup(void);
void test_dup_open(void);

int main(void)
{
    test_dup();
    test_dup_open();
    return 0;
}

void test_dup(void)
{
    int fdts = getdtablesize();
    printf("getdtablesize()=%d\n", fdts);
    assert(256 == fdts);
    int fd = dup(1);
    printf("dup(1)=%d\n", fd);
    assert(3 == fd);
    fd = dup(fd);
    assert(4 == fd);
    printf("dup(3)=%d\n", fd);
    fd = dup2(fd, 6);
    assert(fd == 6);
    printf("dup2(4, 6)=%d\n", fd);
    fd = dup(fd);
    assert(fd == 5);
    printf("dup(6)=%d\n", fd);
}

void test_dup_open(void)
{
    int fd;
    if ((fd = open("/dev/fd/25", O_RDWR)) < 0) {
        err_sys("open /dev/fd/25 with O_RDWR failed.\n");
    }
    printf("fd=open('/dev/fd/25')=%d\n", fd);
}
