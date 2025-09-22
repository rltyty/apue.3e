/**
 i NOTE:
 *
 * Unlike fread(3)/fwrite(3) which is buffered I/O, read(2)/write(2) is
 * non-buffered in user space. No user space buffer, no flush. Thus there
 * is no "stale buffer data before fork()" problem.
 *
 * When fork() is called, file descriptors are duplicated in child process.
 * Both parent and child have separated file descriptors that refer to the
 * ***same file open file description*** in the kernel. This means they share
 * the same file offset.
 * There's no "stale" user-space buffer data to worry about being duplicated.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "rltapue.h"

// Initialize file using a file descriptor
int init_file_fd(const char *const path) {
    if (!path) return -1;
    // Open for reading/writing, create if doesn't exist, truncate to zero
    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) my_perror("error: open");

    int value = 0;
    if (write(fd, &value, sizeof(int)) != sizeof(int)) {
        my_perror("error: write()");
    }
    return fd;
}

// Increment the integer in the file using the file descriptor
int incre_file_fd(int fd) {
    lseek(fd, 0, SEEK_SET); // move to the start before read
    int value;
    if (read(fd, &value, sizeof(int)) != sizeof(int)) {
        my_perror("error: read()");
    }

    lseek(fd, 0, SEEK_SET); // move to the start before write
    value++;
    if (write(fd, &value, sizeof(int)) != sizeof(int)) {
        my_perror("error: write()");
    }

    // No need for fsync here for this example, write is usually sufficient
    // but fsync(fd) would be the equivalent of fflush.
    return value;
}

void test_fd(void) {
    const char *const file = "tmp/data/signals/sharedfile_fd";
    int fd = init_file_fd(file);
    const int rounds = 13;

    TELL_WAIT();
    pid_t pid = fork();

    if (pid < 0) {
        my_perror("error: fork()");
    } else if (pid == 0) { // child
        for (int i = 0; i < rounds; i++) {
            WAIT_PARENT();
            // critical section
            printf("Child: incrementing, value: %d\n", incre_file_fd(fd));
            TELL_PARENT(getppid());
        }
        exit(0);
    } else { // parent
        for (int i = 0; i < rounds; i++) {
            // critical section
            printf("Parent: incrementing, value: %d\n", incre_file_fd(fd));
            TELL_CHILD(pid);
            WAIT_CHILD();
        }
    }

    close(fd); // Close the file descriptor
}

int main(int argc, char **argv) {
    test_fd();
    return 0;
}


/*

> ./Debug/signals/Ex10_6_procsync.fd
Parent: incrementing, value: 1
Child: incrementing, value: 2
Parent: incrementing, value: 3
Child: incrementing, value: 4
Parent: incrementing, value: 5
Child: incrementing, value: 6
Parent: incrementing, value: 7
Child: incrementing, value: 8
Parent: incrementing, value: 9
Child: incrementing, value: 10
Parent: incrementing, value: 11
Child: incrementing, value: 12
Parent: incrementing, value: 13
Child: incrementing, value: 14
Parent: incrementing, value: 15
Child: incrementing, value: 16
Parent: incrementing, value: 17
Child: incrementing, value: 18
Parent: incrementing, value: 19
Child: incrementing, value: 20
Parent: incrementing, value: 21
Child: incrementing, value: 22
Parent: incrementing, value: 23
Child: incrementing, value: 24
Parent: incrementing, value: 25
Child: incrementing, value: 26

> hexdump -C tmp/data/signals/sharedfile_fd

00000000  1a 00 00 00                                       |....|
00000004

> hexdump tmp/data/signals/sharedfile_fd
0000000 001a 0000
0000004

 */


