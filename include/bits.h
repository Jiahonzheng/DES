#ifndef BITS_H_
#define BITS_H_

#include <stdint.h>

#define ODD_PARITY 0xFF
#define EVEN_PARITY 0x00

uint64_t to_uint64(char *data);

void to_bit(uint64_t data, char *c);

int get_bit(uint64_t data, int bit);

uint64_t left_shift(uint64_t data, int len, int bits);

int count_bits(uint64_t data, uint64_t mask);

#endif  // BITS_H_