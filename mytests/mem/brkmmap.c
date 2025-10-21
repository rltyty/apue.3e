#include "rltapue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGESIZE 4096

void test_sbrk(void) {
  void *sb1 = sbrk(4096);
  void *sb2 = sbrk(0);
  void *sb3 = sbrk(256);
  void *sb4 = sbrk(5000);
  void *sb5 = sbrk(1);
  void *sb6 = sbrk(0);

  printf("sb1: %p\n", sb1);
  printf("sb2: %p\n", sb2);
  printf("sb3: %p\n", sb3);
  printf("sb4: %p\n", sb4);
  printf("sb5: %p\n", sb5);
  printf("sb6: %p\n", sb6);
}

void test_mmap(void) {
  // Allocate 4096 bytes (one page) using mmap
  void *ptr = mmap(NULL,                        // Let OS choose address
                   PAGESIZE,                    // Size: 4096 bytes = 4KB
                   PROT_READ | PROT_WRITE,      // Read/write permissions
                   MAP_PRIVATE | MAP_ANONYMOUS, // Private, not file-backed
                   -1,                          // No file descriptor
                   0);                          // No offset

  if (ptr == MAP_FAILED) {
    my_perror("mmap failed");
  }

  printf("mmap allocate memory at: %p\n", ptr);

  // Use the memory
  strcpy((char *)ptr, "Hello from mmap!");
  printf("Content: %s\n", (char *)ptr);

  // Free the memory
  if (munmap(ptr, PAGESIZE) == -1) {
    my_perror("munmap failed");
  }

  printf("Memory unmapped successfully\n");
}

void test_malloc() {
  void *m1 = malloc(0);
  void *m2 = malloc(1);
  void *m3 = malloc(256);
  void *m4 = malloc(PAGESIZE);
  void *m5 = malloc(5000);
  void *m6 = malloc(0);

  printf("m1: %p\n", m1);
  printf("m2: %p\n", m2);
  printf("m3: %p\n", m3);
  printf("m4: %p\n", m4);
  printf("m5: %p\n", m5);
  printf("m6: %p\n", m6);

  free(m1);
  free(m2);
  free(m3);
  free(m4);
  free(m5);
  free(m6);
}

int main(void) {
  test_sbrk();
  test_mmap();
  test_malloc();
}
/*
On Linux:

strace ./Debug/mem/brkmmap 2> x
sb1: 0x55d732a0d000
sb2: 0x55d732a0e000
sb3: 0x55d732a0e000
sb4: 0x55d732a0e100
sb5: 0x55d732a0f488
sb6: 0x55d732a0f489
mmap allocate memory at: 0x7f2b0d635000
Content: Hello from mmap!
Memory unmapped successfully
m1: 0x55d732a0fb40
m2: 0x55d732a0fb60
m3: 0x55d732a0fb80
m4: 0x55d732a0fc90
m5: 0x55d732a10ca0
m6: 0x55d732a12030

> cat x
execve("./Debug/mem/brkmmap", ["./Debug/mem/brkmmap"], 0x7ffcfd32b150 ...) = 0
brk(NULL)                               = 0x55d732a0d000
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f2b0d636000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
newfstatat(3, "", {st_mode=S_IFREG|0644, st_size=36791, ...}, AT_EMPTY_PATH) = 0
mmap(NULL, 36791, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7f2b0d62d000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\20t\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
newfstatat(3, "", {st_mode=S_IFREG|0755, st_size=1926232, ...}, AT_EMPTY_PATH) = 0
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
mmap(NULL, 1974096, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7f2b0d44b000
mmap(0x7f2b0d471000, 1400832, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x26000) = 0x7f2b0d471000
mmap(0x7f2b0d5c7000, 339968, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x17c000) = 0x7f2b0d5c7000
mmap(0x7f2b0d61a000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1cf000) = 0x7f2b0d61a000
mmap(0x7f2b0d620000, 53072, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7f2b0d620000
close(3)                                = 0
mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f2b0d448000
arch_prctl(ARCH_SET_FS, 0x7f2b0d448740) = 0
set_tid_address(0x7f2b0d448a10)         = 2327291
set_robust_list(0x7f2b0d448a20, 24)     = 0
rseq(0x7f2b0d449060, 0x20, 0, 0x53053053) = 0
mprotect(0x7f2b0d61a000, 16384, PROT_READ) = 0
mprotect(0x55d700ecc000, 4096, PROT_READ) = 0
mprotect(0x7f2b0d669000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0x7f2b0d62d000, 36791)           = 0
brk(NULL)                               = 0x55d732a0d000
brk(0x55d732a0e000)                     = 0x55d732a0e000
brk(0x55d732a0e100)                     = 0x55d732a0e100
brk(0x55d732a0f488)                     = 0x55d732a0f488
brk(0x55d732a0f489)                     = 0x55d732a0f489
newfstatat(1, "", {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0x1a), ...}, AT_EMPTY_PATH) = 0
getrandom("\x9f\x55\xbd\x2a\xd3\x7d\xed\x12", 8, GRND_NONBLOCK) = 8
brk(0x55d732a30489)                     = 0x55d732a30489
brk(0x55d732a31000)                     = 0x55d732a31000
write(1, "sb1: 0x55d732a0d000\n", 20)   = 20
write(1, "sb2: 0x55d732a0e000\n", 20)   = 20
write(1, "sb3: 0x55d732a0e000\n", 20)   = 20
write(1, "sb4: 0x55d732a0e100\n", 20)   = 20
write(1, "sb5: 0x55d732a0f488\n", 20)   = 20
write(1, "sb6: 0x55d732a0f489\n", 20)   = 20
mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f2b0d635000
write(1, "mmap allocate memory at: 0x7f2b0"..., 40) = 40
write(1, "Content: Hello from mmap!\n", 26) = 26
munmap(0x7f2b0d635000, 4096)            = 0
write(1, "Memory unmapped successfully\n", 29) = 29
write(1, "m1: 0x55d732a0fb40\n", 19)    = 19
write(1, "m2: 0x55d732a0fb60\n", 19)    = 19
write(1, "m3: 0x55d732a0fb80\n", 19)    = 19
write(1, "m4: 0x55d732a0fc90\n", 19)    = 19
write(1, "m5: 0x55d732a10ca0\n", 19)    = 19
write(1, "m6: 0x55d732a12030\n", 19)    = 19
exit_group(0)                           = ?
+++ exited with 0 +++
 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
