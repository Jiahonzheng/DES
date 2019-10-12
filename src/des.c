#include "des.h"
#include <stdlib.h>
#include "tables.h"

#define L64_MASK 0x00000000ffffffff
#define H64_MASK 0xffffffff00000000

uint64_t generate_des_key() {
  char key[8];
  for (int i = 0; i < 8; ++i) {
    key[i] = rand() & 0xFE;
    key[i] |= count_bits(key[i], 0xFF) % 2 != 1 ? 1 : 0;
  }
  return to_uint64(key);
}

bool verify_des_key(uint64_t key) {
  char c[8];
  to_bit(key, c);
  for (int i = 0; i < 8; ++i) {
    if (count_bits(c[i], 0xFF) % 2 != 1) {
      return false;
    }
  }
  return true;
}

uint64_t do_permutation(const char* perm, int size, int to, uint64_t chunk) {
  uint64_t ret = 0;
  for (int i = 0; i < to; i++) {
    ret = (ret << 1) | ((chunk >> (size - perm[i])) & 1);
  }
  return ret;
}

void calc_subkeys(uint64_t key, uint64_t* subkeys) {
  key = do_permutation(PERM_REMOVE_PARITY, 64, 56, key);
  uint64_t permuted_choice_1 = do_permutation(PC1, 64, 56, key);
  uint32_t C = (uint32_t)((permuted_choice_1 >> 28) & 0xFFFFFFF);
  uint32_t D = (uint32_t)(permuted_choice_1 & 0xFFFFFFF);
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < iteration_shift[i]; ++j) {
      C = (0x0fffffff & (C << 1)) | (0x00000001 & (C >> 27));
      D = (0x0fffffff & (D << 1)) | (0x00000001 & (D >> 27));
    }
    uint64_t permuted_choice_2 = (((uint64_t)C) << 28) | (uint64_t)D;
    subkeys[i] = 0;
    for (int j = 0; j < 48; ++j) {
      subkeys[i] = (subkeys[i] << 1) | ((permuted_choice_2 >> (56 - PC2[j])) & 1);
    }
  }
}

uint64_t calc_sbox(uint64_t data) {
  uint64_t ret = 0;
  for (int i = 0; i < 8; ++i) {
    char row = (char)((data & (0x0000840000000000 >> (6 * i))) >> (42 - 6 * i));
    row = (row >> 4) | (row & 1);
    char column =
        (char)((data & (0x0000780000000000 >> (6 * i))) >> (43 - 6 * i));

    ret = (ret << 4) | (uint32_t)(S[i][16 * row + column] & 0xF);
  }
  return ret;
}

uint64_t des_block(uint64_t chunk, uint64_t* subkeys, int mode);

void _des(int mode, uint64_t* subkeys, char* read_chunk, char* write_chunk,
          FILE* output_file) {
  uint64_t ret = des_block(to_uint64(read_chunk), subkeys, mode);
  to_bit(ret, write_chunk);
  fwrite(write_chunk, sizeof(uint8_t), 8, output_file);
}

int des(int mode, char* key_fname, char* input_fname, char* output_fname) {
  FILE* key_file = fopen(key_fname, "rb");
  char key_bits[8];
  if (key_file == NULL ||
      fread(key_bits, sizeof(uint8_t), KEY_SIZE, key_file) != KEY_SIZE) {
    return -1;
  }
  fclose(key_file);
  uint64_t key = to_uint64(key_bits);
  if (verify_des_key(key) == false) {
    return -1;
  }

  FILE* input_file = fopen(input_fname, "rb");
  if (input_file == NULL) {
    return -1;
  }
  fseek(input_file, 0, SEEK_END);
  long input_len = ftell(input_file);
  fseek(input_file, 0, SEEK_SET);

  FILE* output_file = fopen(output_fname, "wb+");
  if (output_file == NULL) {
    return -1;
  }

  uint64_t subkeys[16] = {0};
  calc_subkeys(key, subkeys);

  char read_chunk[8];
  char write_chunk[8];
  size_t read_chunk_len;
  while ((read_chunk_len = fread(read_chunk, sizeof(uint8_t), 8, input_file))) {
    input_len -= read_chunk_len;

    _des(mode, subkeys, read_chunk, write_chunk, output_file);

    if (mode == ENCRYPT_MODE && input_len < 8) {
      fread(read_chunk, sizeof(uint8_t), 8, input_file);
      for (int i = input_len; i < 8; i++) {
        read_chunk[i] = input_len;
      }
      _des(ENCRYPT_MODE, subkeys, read_chunk, write_chunk, output_file);
      break;
    }

    if (mode == DECRYPT_MODE && input_len == 8) {
      fread(read_chunk, sizeof(uint8_t), 8, input_file);
      uint64_t ret = des_block(to_uint64(read_chunk), subkeys, DECRYPT_MODE);
      int times = ret & 0x9;
      to_bit(ret, write_chunk);
      fwrite(write_chunk, sizeof(uint8_t), times, output_file);
      break;
    }
  }

  fclose(input_file);
  fclose(output_file);
  return 1;
}

uint64_t des_block(uint64_t chunk, uint64_t* subkeys, int mode) {
  uint64_t init_perm_res = do_permutation(IP, 64, 64, chunk);
  uint32_t L = (uint32_t)(init_perm_res >> 32) & L64_MASK;
  uint32_t R = (uint32_t)init_perm_res & L64_MASK;

  for (int i = 0; i < 16; ++i) {
    uint64_t sbox_input = do_permutation(E, 32, 48, R);

    if (mode == DECRYPT_MODE) {
      sbox_input ^= subkeys[15 - i];
    } else {
      sbox_input ^= subkeys[i];
    }

    uint64_t sbox_output = calc_sbox(sbox_input);
    uint64_t feistel_ret = do_permutation(P, 32, 32, sbox_output);

    uint32_t temp = R;
    R = L ^ feistel_ret;
    L = temp;
  }

  uint64_t pre_output = (((uint64_t)R) << 32) | (uint64_t)L;
  uint64_t ret = do_permutation(PI, 64, 64, pre_output);
  return ret;
}
