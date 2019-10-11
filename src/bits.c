#include "bits.h"

uint64_t to_uint64(char* data) {
  uint64_t res = 0;
  for (int i = 0; i < 8; ++i) {
    res |= ((uint64_t)data[i] & ODD_PARITY) << ((7 - i) * 8);
  }
  return res;
}

int get_bit(uint64_t data, int bit) { return (data >> bit) & 1; }

void to_char(uint64_t data, char* c) {
  for (int i = 0; i < 8; ++i) {
    c[i] = (data >> ((7 - i) * 8)) & ODD_PARITY;
  }
}

int count_bits(uint64_t data, uint64_t mask) {
  int ret = 0;
  data &= mask;
  for (; data; data -= data & -data) {
    ++ret;
  }
  return ret;
}

uint64_t left_shift(uint64_t data, int len, int bits) {
  for (int i = 0; i < bits; ++i) {
    data = (data << 1) | ((data >> (len - 1)) & 1);
  }
  return data & ~(~0 << len);
}