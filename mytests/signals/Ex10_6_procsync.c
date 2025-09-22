/**
 * IPC using signals
 *
 * Write the following program to test the parent–child synchronization
 * functions in Figure 10.24. The process creates a file and writes the
 * integer 0 to the file. The process then calls fork, and the parent and
 * child alternate incrementing the counter in the file. Each time the counter
 * is incremented, print which process (parent or child) is doing the
 * increment.
 *
 *
 * NOTE:
 *
 * fflush() stream buffer BEFORE fork()!!! Otherwise there will be another
 * integer 0 written in the file.
 *
 * > hexdump tmp/data/signals/sharedfile
 * 0000000 001a 0000 0000 0000
 * 0000008
 *
 * Reason:
 *
 * The problem lies in the child's first run of incre_file() and how fseek()
 * interacts with the child's inherited, unflushed buffer.
 *
 * https://pubs.opengroup.org/onlinepubs/9799919799/functions/fopen.html
 *
 * "When a file is opened with update mode ('+' in the mode argument), both
 * input and output can be performed on the associated stream. However, the
 * application shall ensure that output is not directly followed by input
 * without an intervening call to fflush() or to a file positioning function
 * (fseek(), fsetpos(), or rewind()), and input is not directly followed by
 * output without an intervening call to a file positioning function, unless
 * the input operation encounters end-of-file."
 *
 * "If the stream is writable and buffered data had not been written to the
 * underlying file, fseek() shall cause the unwritten data to be written to
 * the file and shall mark the st_ctime and st_mtime fields of the file for
 * update."

 * fseek(pf, 0, SEEK_SET); (== rewind(pf);)
 *
 * 1. init_file() calls fwrite() to write an integer 0. If there's no
 * fflush(), this 0 sits in the stdio user-space buffer. When fork() is
 * called, both the parent and child processes inherit an identical copy of
 * this memory, which includes this unflushed buffer containing the integer 0

 * 2. The Parent's Turn:
 * fseek(pf, 0, SEEK_SET): The parent's stream has pending output (the 0 in
 * its buffer). To fulfill the fseek request, the stdio library must flush the
 * buffer first. At this point, the kernel file offset is at 0, so the 0 is
 * written to the start of the file. The file now contains 00 00 00 00. The
 * fseek then completes, leaving the offset at 0.

 * fread(): Reads the 0 from the file. The kernel's file offset is now at
 * position 4.

 * fseek(pf, 0, SEEK_SET): Rewinds the kernel offset back to 0.

 * fwrite(): The parent increments the value to 1 and writes it into its
 * buffer.

 * fflush(): The parent's buffer (containing 1) is written to the file at
 * offset 0, overwriting the original 0.

 * Result of Parent's Turn: The file on disk contains the 4-byte integer 1.
 * The shared kernel file offset is now at position 4.

 * 3. The Child's Turn (The Critical Moment) This is where the 8-byte problem
 * occurs, just as you reasoned.

 * Child Wakes Up: The child begins its first run of incre_file(). Crucially,
 * its stdio buffer still contains the original, stale integer 0 that it
 * inherited from before the fork().

 * fseek(pf, 0, SEEK_SET): The child's stream also has pending output (the
 * stale 0). To handle the seek, the stdio library must flush this buffer
 * first. But where does it flush? It flushes to the current kernel file
 * offset, which the parent just left at position 4.

 * The Append: The child's stale 0 is written to the file starting at
 * offset 4. The file now contains the integer 1 (from the parent) followed
 * by the integer 0 (from the child's stale buffer). The file is now 8 bytes
 * long.

 * The Rest of the Function: The fseek now completes, moving the kernel offset
 * to 0. The child then fread()s the integer 1 from the start of the file,
 * increments it to 2, seeks back to 0, and fwrite()s/fflush()es the 2,
 * overwriting the 1.

 * The Final Result After the first round, the file's contents are:

 * Bytes 0-3: The integer 2
 * Bytes 4-7: The integer 0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rltapue.h"

FILE* init_file(const char *const path) {
  if (!path) return NULL;
  // Use "w+b" or "wb+" for binary read/write mode
  FILE *pf = fopen(path, "wb+");
  if (!pf) my_perror("error: fopen");

  int value = 0;
  if (fwrite(&value, sizeof(int), 1, pf) < 1) {
    if (feof(pf)) {
      printf("End of file reached.\n");
    } else if (ferror(pf)) {
      printf("Read error occurred (ferror set).\n");
    }
    my_perror("error: fwrite()");
  }
  fflush(pf); // !!! Flush the buffer to disk BEFORE fork() !!!
  return pf;
}

int incre_file(FILE *pf) {
  fseek(pf, 0, SEEK_SET); // move to the start before read, == rewind(pf);
  int value;
  if (fread(&value, sizeof(int), 1, pf) < 1) {
    if (feof(pf)) {
      printf("End of file reached.\n");
    } else if (ferror(pf)) {
      printf("Read error occurred (ferror set).\n");
    }
    my_perror("error: fwrite()");
  }
  printf("value: %d\n", value);

  fseek(pf, 0, SEEK_SET); // move to the start before write
  value++;
  if (fwrite(&value, sizeof(int), 1, pf) < 1) {
    if (feof(pf)) {
      printf("End of file reached.\n");
    } else if (ferror(pf)) {
      printf("Read error occurred (ferror set).\n");
    }
    my_perror("error: fwrite()");
  }
  fflush(pf);             // make sure push changes to disk
  return value;
}

void test(void) {
  const char *const file = "tmp/data/signals/sharedfile";
  FILE *pf = init_file(file);
  const int rounds = 13;
  TELL_WAIT();
  pid_t pid = fork();
  if (pid < 0) {
    my_perror("error: fork()");
  } else if (pid == 0) {  // child
    for (int i = 0; i < rounds; i++) {
      WAIT_PARENT();
      // critical section
      printf("Child: incrementing, value: %d\n", incre_file(pf));
      TELL_PARENT(getppid());
    }
    exit(0);
  } else {  // parent
    for (int i = 0; i < rounds; i++) {
      // critical section
      printf("Parent: incrementing, value: %d\n", incre_file(pf));
      TELL_CHILD(pid);
      WAIT_CHILD();
    }
  }
  fclose(pf);
}

int main(int argc, char **argv) {
  test();
  return 0;
}

/*

### Running in command line: stdout is line-buffered

> ./Debug/signals/Ex10_6_procsync
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

### Running in Neovim command mode: stdout redirected to fully-buffered pipe

:!Debug/signals/Ex10_6_procsync
Child: incrementing, value: 2
Child: incrementing, value: 4
Child: incrementing, value: 6
Child: incrementing, value: 8
Child: incrementing, value: 10
Child: incrementing, value: 12
Child: incrementing, value: 14
Child: incrementing, value: 16
Child: incrementing, value: 18
Child: incrementing, value: 20
Child: incrementing, value: 22
Child: incrementing, value: 24
Child: incrementing, value: 26
Parent: incrementing, value: 1
Parent: incrementing, value: 3
Parent: incrementing, value: 5
Parent: incrementing, value: 7
Parent: incrementing, value: 9
Parent: incrementing, value: 11
Parent: incrementing, value: 13
Parent: incrementing, value: 15
Parent: incrementing, value: 17
Parent: incrementing, value: 19
Parent: incrementing, value: 21
Parent: incrementing, value: 23
Parent: incrementing, value: 25

Child exits first, its stdout buffer gets flushed first.
Parent waits for the child and exits later, stdout buffer gets flushed later.

### The content of sharedfile

> hexdump -C tmp/data/signals/sharedfile

00000000  1a 00 00 00                                       |....|
00000004
> hexdump tmp/data/signals/sharedfile

0000000 001a 0000
0000004

001a -> 26, the value is correct.

 */
