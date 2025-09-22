/**
 * Automatic array in a function is limited to thread stack size.
 * - `ulimit -s <size>`
 * - `setrlimit(RLIMIT_STACK, ...)`
 *
 * On macOS and Linux (Debian 12):
 * > ulimit -a | grep "\-s"
 * -s: stack size (kbytes)             8192
 */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

#define _1_GB 1024 * 1024 * 1024
#define _1_MB 1024 * 1024
#define _7_MB 1024 * 1024 * 7
#define _8_MB 1024 * 1024 * 8
#define _9_MB 1024 * 1024 * 9

/**
 * Lazy stack allocation, only reserved but not committed (mapped to the
 * physical page) until being accessed.
 */
void stack_size_test0() {
  /* Declare 1GB array w/o initialization: ✅ PASSED */
  char a[_1_GB];
}

void stack_size_test1() {
  /* Declare two 1GB arrays w/o initialization: ✅ PASSED */
  char a[_1_GB];
  char b[_1_GB];
}

void stack_size_test2() {
  /* Declare 1GB array w/o initialization, but touch the page: ❌ FAILED */
  char a[_1_GB];
  a[0] = 1;       // write
  assert(a[0] == 1);
}

void stack_size_test3() {
  /* Declare 1GB array w/o initialization, but touch the page: ❌ FAILED */
  char a[_1_GB];
  int x = a[0];   // read
}

/**
 * The call instruction (call `puts`) pushes a return address onto the stack
 * at the new `rsp`, which is already unmapped. It causes segmentation fault.
 */
void stack_size_test4() {
  /* Declare 1GB array w/o initialization, plus a function call: ❌ FAILED */
  char a[_1_GB];
  puts("Declaration 1GB char array w/o initialization is OK. "
       "But another function call that pushes a return address "
       "at a unmapped location will cause segmentation fault.");
}

void stack_size_test5() {
  /* Declare 1GB array w/o initialization, plus a statement: ❌ FAILED */
  char a[_1_GB];
  int b = 1;
}

/**
 * Initialization of such a large array substantially slows down the
 * compilation.
 * Small .o file vs Large time and space consumption for its generation
 * - The compiler is smart enough not to dump a literal 1 GB blob.
 * - It collapses the array into:
 *   - a .bss reservation (.comm or .lcomm) of 1 GB,
 *   - plus a tiny .data fragment for the non-zero entries.
 * - That’s why the output file size is KB, but getting there required walking
 *   through a GB-scale structure.
 */
void stack_size_test6() {
  /* Initialization an array larger than stack size: ❌ FAILED */
  char a[_1_GB] = {
    0,
    [1] = 1,
    [2] = 2,
    [16 * 1024 * 1024] = 127,
  };
}

void stack_size_test7() {
  /* Initialization an array larger than stack size: ❌ FAILED */
  char a[_1_GB] = {
    0
  };
}

void stack_size_test8() {
  /* Declare 8MB array w/o initialization, plus a byte: ❌ FAILED */
  char a[_8_MB];
  char b = 'x';
}

void stack_size_test9() {
  char a[_8_MB];
  assert(0 == a[_8_MB / 2]);
}

void stack_size_test10() {
  char a[_8_MB];
  char b[1];
  // assert(0 == a[_8_MB / 2]);
}

void stack_size_test11() {
  char a[_7_MB] = {
    0,
    [1] = 1,
    [2] = 2,
    [10] = 10,
    [_7_MB / 2] = 127
  };
  assert(a[_7_MB / 2] == 127);
}

int main(void) {
  printf("LONG_MAX=%ld\n", LONG_MAX);

  stack_size_test0();
  stack_size_test1();

  /* Compile without optimization CFLAGS += -O0 , runtime error will occur.*/
  /* Debug mode: runtime error, Release mode: passed */

  stack_size_test2();        // Runtime error: Segmentation Fault
  stack_size_test3();        // Runtime error: Segmentation Fault
  stack_size_test4();        // Runtime error: Segmentation Fault
  stack_size_test5();        // Runtime error: Segmentation Fault
  stack_size_test6();        // Runtime error: Segmentation Fault
  stack_size_test7();        // Runtime error: Segmentation Fault
  stack_size_test8();        // Runtime error: Segmentation Fault

  stack_size_test9();
  stack_size_test10();
  stack_size_test11();
  return 0;
}


/*
 *
NOTE:
Debug mode: CFLAGS += -O0 (no optimization) -g    : Runtime error
Release mode: CFLAGS += O3                        : No error
Assembly Basic:
1. rsp = Register Stack Pointer
2. rbp = Register Base Pointer
3. On x86-64, the stack grows downward (toward lower addresses):
  rsp ──▶ [ return address   ]
          [ saved registers  ]
          [ local variables  ]  ← stack grows downward
4. At function entry, compilers typically do:
  push rbp         ; save old base pointer
  mov rbp, rsp     ; set new base pointer to current stack pointer
  sub rsp, N       ; reserve N bytes for locals
5. In function:
- rbp stays constant throughout the function.
- Local variables are accessed at fixed offsets from rbp (e.g., [rbp - 0x10]).
6. When the function ends:
  add rsp, N       ; free locals
  pop rbp          ; restore old base pointer
  ret              ; return to caller
7. Summary
High addresses
+---------------------+
| Caller’s stack data |
| Return address      | ← saved by CALL
+---------------------+
| Old RBP             | ← push rbp
+---------------------+
| Local variables     | ← rsp after sub
| ...                 |
+---------------------+
Low addresses
8. function call:
On 64-bit Linux and macOS, the calling convention says:
- RDI = 1st function argument
- RSI = 2nd argument
- RDX = 3rd argument
- RCX = 4th argument
- R8 = 5th argument
- R9 = 6th argument
Example:
  lea rdi, [rip+0x0]    ; loads the address of your string literal into
                        ; RDI (the 1st parameter for puts).
  call 87 <stack_size_test4+0x17> ; pushing the return address (87) onto the stack
9. At high optimization levels like -O3, the compiler can completely change
the implementation of a function, as long as it obeys the “as-if” rule from
the C standard:
- The compiler can optimize, reorder, or even remove code as long as the
program behaves the same as if it were executed according to the original
source, with respect to observable behavior.
Thus, when compile in Release mode, from the perspective of compiler:
- stack_size_test0() has a huge array but never uses it → dead → remove.
- stack_size_test1() has two huge arrays but never uses them → dead → remove.
- stack_size_test2() has a huge array and you assign a[0]=1, but the value is
never used anywhere else, so the store has no observable effect from the
outside → dead → remove.

## Object dump (Debug mode):
> objdump -d -M intel ./Debug/standards/test_limits.o > od
On Linux with GCC:

0000000000000000 <stack_size_test0>:
   0:   55                      push   rbp
   1:   48 89 e5                mov    rbp,rsp
   4:   5d                      pop    rbp
   5:   c3                      ret
   6:   66 2e 0f 1f 84 00 00    cs nop WORD PTR [rax+rax*1+0x0]
   d:   00 00 00

0000000000000010 <stack_size_test1>:
  10:   55                      push   rbp
  11:   48 89 e5                mov    rbp,rsp
  14:   5d                      pop    rbp
  15:   c3                      ret
  16:   66 2e 0f 1f 84 00 00    cs nop WORD PTR [rax+rax*1+0x0]
  1d:   00 00 00

0000000000000020 <stack_size_test2>:
  20:   55                      push   rbp
  21:   48 89 e5                mov    rbp,rsp
  24:   48 81 ec 80 ff ff 3f    sub    rsp,0x3fffff80
  2b:   c6 85 00 00 00 c0 01    mov    BYTE PTR [rbp-0x40000000],0x1
  32:   48 81 c4 80 ff ff 3f    add    rsp,0x3fffff80
  39:   5d                      pop    rbp
  3a:   c3                      ret
  3b:   0f 1f 44 00 00          nop    DWORD PTR [rax+rax*1+0x0]

...

0000000000000070 <stack_size_test4>:
  70:   55                      push   rbp
  71:   48 89 e5                mov    rbp,rsp
  74:   48 81 ec 00 00 00 40    sub    rsp,0x40000000
  7b:   48 8d 3d 00 00 00 00    lea    rdi,[rip+0x0]        # 82 <stack_size_test4+0x12>
  82:   e8 00 00 00 00          call   87 <stack_size_test4+0x17>
  87:   48 81 c4 00 00 00 40    add    rsp,0x40000000
  8e:   5d                      pop    rbp
  8f:   c3                      ret

...


## Object dump (Release mode):
> objdump -d -M intel ./Release/standards/test_limits.o > od.rel

0000000000000000 <stack_size_test0>:
   0:	c3                   	ret
   1:	66 66 66 66 66 66 2e 	data16 data16 data16 data16 data16 cs nop WORD PTR [rax+rax*1+0x0]
   8:	0f 1f 84 00 00 00 00
   f:	00

0000000000000010 <stack_size_test1>:
  10:	c3                   	ret
  11:	66 66 66 66 66 66 2e 	data16 data16 data16 data16 data16 cs nop WORD PTR [rax+rax*1+0x0]
  18:	0f 1f 84 00 00 00 00
  1f:	00

0000000000000020 <stack_size_test2>:
  20:	c3                   	ret
  21:	66 66 66 66 66 66 2e 	data16 data16 data16 data16 data16 cs nop WORD PTR [rax+rax*1+0x0]
  28:	0f 1f 84 00 00 00 00
  2f:	00

0000000000000030 <stack_size_test3>:
  30:	c3                   	ret
  31:	66 66 66 66 66 66 2e 	data16 data16 data16 data16 data16 cs nop WORD PTR [rax+rax*1+0x0]
  38:	0f 1f 84 00 00 00 00
  3f:	00

0000000000000040 <stack_size_test4>:
  40:	48 8d 3d 00 00 00 00 	lea    rdi,[rip+0x0]        # 47 <stack_size_test4+0x7>
  47:	e9 00 00 00 00       	jmp    4c <stack_size_test4+0xc>
  4c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

...

 */
