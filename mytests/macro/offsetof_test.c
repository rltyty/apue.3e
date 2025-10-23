#include <assert.h>

/**
 * <stddef.h> provides macro `offsetof` based on compiler implementation
 * `__builtin_offsetof(TYPE, MEMBER)`.
 *
 * 1. `(TYPE*)0` pretends there's an object of type TYPE located at address 0.
 * 2. Why is `&((TYPE *)0)->MEMBER` safe and does it not dereference?
 *    `&((TYPE *)0)->MEMBER` looks like it’s taking a member's address
 *    through a null pointer, but:
 *    - The compiler never emits a memory load or store.
 *    - It simply computes the address offset statically from the structure's
 *      layout.
 *
 * NOTE: `sizeof` and Data alignment:
 * - $$\text{sizeof(struct S)} = \sum_{i=1}^{n}(\text{padding\_before}(m_i) + \text{sizeof}(m_i))$$
 * - $padding\_before(m_i)$: inserted before member $m_i$ to ensure its starting address aligns with its type's alignment.
 * - $$\text{offset}(m_i) = \text{unaligned\_offset}(m_i) + \text{padding\_before}(m_i)$$
 * - A TYPE with alignment N must be stored at an address that's a multiple of N.
 * - Data alignment ensures efficient CPU access.
 * - Typical alignment rules on x86-64 (System V ABI):
| Type                            | Size | Alignment                       | Why                                |
| ------------------------------- | ---- | ------------------------------- | ---------------------------------- |
| `char`, `int8_t`                | 1 B  | 1 B                             | Can start anywhere                 |
| `short`, `int16_t`              | 2 B  | 2 B                             | Must align to 2 B                  |
| `int`, `float`                  | 4 B  | 4 B                             | Word-aligned for 32-bit access     |
| `long`, `long long`, `double`   | 8 B  | 8 B                             | 64-bit CPU registers are 8 B       |
| `pointer`(`void *`, etc.)       | 8 B  | 8 B                             | Pointers are 8 B on 64-bit systems |
| Structs                         | —    | largest alignment among members | So members stay aligned            |

 */

#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->MEMBER)
#define container_of(PTR, TYPE, MEMBER) \
        ((TYPE*) ((char*)(PTR) - offsetof(TYPE, MEMBER)))

struct list_head {
  struct list_head *prev;
  struct list_head *next;
};

struct task {
  int pid;
  char a[5];
  struct list_head list;
  char b[9];
  struct list_head list2;
  char buf[4096];
  char c[1];
};

void test_struct_offset() {
  // use offsetof macro                                      size  offset + padding = aligned_offset (adjusted)
  assert(0 == offsetof(struct task, pid));   // sizeof(pid)    4    0
  assert(4 == offsetof(struct task, a));     // sizeof(a)      5    4
  assert(16 == offsetof(struct task, list)); // sizeof(list)   16   9     + 7 = 16
  assert(32 == offsetof(struct task, b));    // sizeof(b)      9    32
  assert(48 == offsetof(struct task, list2));// sizeof(list2)  16   41    + 7 = 48
  assert(64 == offsetof(struct task, buf));  // sizeof(buf)    4096 64
  assert(4160 == offsetof(struct task, c));  // sizeof(c)      1    4160
                                             // end of struct task: 4161  + 7 = 4168
                                             // sizeof(task):4168, padding:21

  // use glibc/clang syntactic extension
  assert(16 == __builtin_offsetof(struct task, list));
  assert(48 == __builtin_offsetof(struct task, list2));
  assert(4160 == __builtin_offsetof(struct task, c));

  // subtraction to go from a member pointer to the container pointer
  struct task t;
  assert(&t == (struct task*)(&t.c - offsetof(struct task, c)));
  assert(&t == (struct task*)(&t.c - __builtin_offsetof(struct task, c)));
  assert(&t == container_of(&t.c, struct task, c));
  assert(&t == container_of(&t.list, struct task, list));

  // No dereference, no segmentation fault
  assert(4 == sizeof(((struct task*)0)->pid));
  assert(5 == sizeof(((struct task*)0)->a));
  assert(16 == sizeof(((struct task*)0)->list));
  assert(9 == sizeof(((struct task*)0)->b));
  assert(16 == sizeof(((struct task*)0)->list2));
  assert(4096 == sizeof(((struct task*)0)->buf));
  assert(1 == sizeof(((struct task*)0)->c));
  assert(4147 == 4 + 5 + 16 + 9 + 16 + 4096 + 1);
  assert(4168 == sizeof(struct task)); // 4168 - 4147 = 21 = (16-9) + (48-41)
                                       // + (4168-4161)
}

int main(int argc, char *argv[]) {
  test_struct_offset();
  return 0;
}
