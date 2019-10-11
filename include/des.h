#ifndef DES_H_
#define DES_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ENCRYPT_MODE 0
#define DECRYPT_MODE 1

#define KEY_SIZE 8

uint64_t generate_des_key();

bool verify_des_key(uint64_t key);

int des(int mode, char* key_fname, char* input_fname, char* output_fname);

uint64_t des_block(uint64_t chunk, uint64_t key, int mode);

void validate_des();

#endif  // DES_H_P