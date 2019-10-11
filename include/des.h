#ifndef DES_H_
#define DES_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ENCRYPT_MODE 0
#define DECRYPT_MODE 1

uint64_t generate_des_key();

bool verify_des_key(uint64_t key);

uint64_t des(uint64_t chunk, uint64_t key, int mode);

void validate_des();

#endif  // DES_H_