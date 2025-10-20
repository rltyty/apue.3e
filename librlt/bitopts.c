#include <stdio.h>
#include "rltapue.h"

/**
 * NOTE:
 * 1. -value = ~value + 1 = ~(value - 1)
 * 2. All GCC/Clang built-ins - no headers required
 *    int pos1 = __builtin_ffs(x);        // Find First Set (1-based)
 *    int lz = __builtin_clz(x);          // Count Leading Zeros
 *    int tz = __builtin_ctz(x);          // Count Trailing Zeros
 *    int pop = __builtin_popcount(x);    // Population Count (set bits)
 *    // 64-bit versions
 *    int pos2 = __builtin_ffsll(x);      // For long long types
 *    int lz2 = __builtin_clzll(x);
 */

/* Core bit operations */
uint32_t bit_set(uint32_t value, uint8_t n) { return value | (1U << n); }

uint32_t bit_clear(uint32_t value, uint8_t n) { return value & ~(1U << n); }

uint32_t bit_toggle(uint32_t value, uint8_t n) { return value ^ (1U << n); }

bool bit_test(uint32_t value, uint8_t n) {
  // return (value >> n) & 1U;  // ISO C11: bool: true: 1, false: 0
  return (value & (1U << n)) != 0;  // a little faster
}

/* LSB operations */
uint32_t bit_lsb_value(uint32_t value) {  // Returns mask of lowest set bit
  return -value & value;
}

int8_t bit_lsb_position(uint32_t value) {  // Returns position (-1 if none)
  if (value == 0) return -1;
  return __builtin_ffs(value) - 1;  // compiler intrinsic function of Clang/GCC
}

uint32_t bit_clear_lsb(uint32_t value) {  // Removes lsb, returns what remains.
  return value & (value - 1);
}

// Utility functions
bool bit_is_power_of_two(uint32_t value) {
  // 0 is not a power of 2 and after clears the lsb, what remains should be 0
  return value && !(value & (value - 1));
}

uint8_t bit_count_set(uint32_t value) {  // Population count
  uint8_t count = 0;
  while (value > 0) {
    value &= value - 1; // clears the lsb
    count++;
  }
  return count;
}

/* Alignment operations */
uint32_t bit_align_up(uint32_t value, uint32_t alignment) {
  if (alignment == 0 && !bit_is_power_of_two(alignment)) return -1;
  return (value + alignment - 1) & ~(alignment - 1);
}

uint32_t bit_align_down(uint32_t value, uint32_t alignment) {
  if (!bit_is_power_of_two(alignment)) return -1;
  // return value & ~(alignment - 1);
  return value & -alignment;
}

#ifdef test_bitopts
#undef test_bitopts
#include <assert.h>

void test_bit_set() {
  assert(1 == bit_set(0, 0));
  assert(2 == bit_set(0, 1));
  assert(4 == bit_set(0, 2));
  assert(256 == bit_set(0, 8));
  assert((1 << 8) == bit_set(0, 8));
  assert(65536 == bit_set(0, 16));
  assert((1 << 16) == bit_set(0, 16));

  assert(7 == bit_set(6, 0));
  assert(7 == bit_set(5, 1));
  assert(7 == bit_set(3, 2));
}

void test_bit_clear() {
  assert(0 == bit_clear(1, 0));
  assert(0 == bit_clear(2, 1));
  assert(0 == bit_clear(4, 2));
  assert(0 == bit_clear(256, 8));
  assert(0 == bit_clear(65536, 16));

  assert(6 == bit_clear(7, 0));
  assert(5 == bit_clear(7, 1));
  assert(3 == bit_clear(7, 2));
}

void test_bit_toggle() {
  assert(5 == bit_toggle(7, 1));
  assert(7 == bit_toggle(5, 1));
  assert(1 == bit_toggle(0, 0));
  assert(0 == bit_toggle(1, 0));
  assert(128 == bit_toggle(0, 7));
}

void test_bit_test() {
  assert(bit_test(5, 0));
  assert(!bit_test(5, 1));
  assert(bit_test(5, 2));
}

void test_bit_lsb_value() {
  assert(2 == bit_lsb_value(0b1010));
  assert(4 == bit_lsb_value(0b1100));
  assert(1 == bit_lsb_value(0b1001));
}

void test_bit_lsb_position() {
  assert(0 == bit_lsb_position(0b1001));
  assert(1 == bit_lsb_position(0b1010));
  assert(2 == bit_lsb_position(0b1100));
}

void test_bit_clear_lsb() {
  assert(0b1000 == bit_clear_lsb(0b1001));
  assert(0b1000 == bit_clear_lsb(0b1010));
  assert(0b1000 == bit_clear_lsb(0b1100));
}

void test_bit_align_up() {
  assert(8192 == bit_align_up(5000, 4096));
  assert(128 == bit_align_up(65, 64));
}

void test_bit_align_down() {
  assert(4096 == bit_align_down(5000, 4096));
  assert(64 == bit_align_down(65, 64));
}

void test_bit_is_power_of_two() {
  assert(bit_is_power_of_two(4096));
  assert(bit_is_power_of_two(128));
  assert(bit_is_power_of_two(2));
  assert(bit_is_power_of_two(1));
  assert(!bit_is_power_of_two(0));
  assert(!bit_is_power_of_two(3));
  assert(!bit_is_power_of_two(6));
}

void test_bit_count_set() {
  assert(3 == bit_count_set(7));
  assert(2 == bit_count_set(6));
  assert(2 == bit_count_set(5));
  assert(1 == bit_count_set(4));
}

int main(void) {
  test_bit_set();
  test_bit_clear();
  test_bit_toggle();
  test_bit_test();
  test_bit_lsb_value();
  test_bit_lsb_position();
  test_bit_clear_lsb();
  test_bit_align_up();
  test_bit_align_down();
  test_bit_is_power_of_two();
  test_bit_count_set();
}
#endif
