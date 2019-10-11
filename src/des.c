#include "des.h"
#include <stdlib.h>
#include "bits.h"
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
  to_char(key, c);
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

void calc_subkey(uint64_t key, uint64_t* subkey) {
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
    subkey[i] = 0;
    for (int j = 0; j < 48; ++j) {
      subkey[i] = (subkey[i] << 1) | ((permuted_choice_2 >> (56 - PC2[j])) & 1);
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

uint64_t des(uint64_t chunk, uint64_t key, int mode) {
  uint64_t init_perm_res = do_permutation(IP, 64, 64, chunk);
  uint32_t L = (uint32_t)(init_perm_res >> 32) & L64_MASK;
  uint32_t R = (uint32_t)init_perm_res & L64_MASK;

  uint64_t subkey[16] = {0};
  calc_subkey(key, subkey);

  for (int i = 0; i < 16; ++i) {
    uint64_t sbox_input = do_permutation(E, 32, 48, R);

    if (mode == DECRYPT_MODE) {
      sbox_input ^= subkey[15 - i];
    } else {
      sbox_input ^= subkey[i];
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

void validate_des() {
  int i;
  uint64_t result = 0xa7f1d92a83c8d9fe;
  uint64_t key = 0xa7f1d92a83c8d9fe;

  uint64_t e = des(result, key, ENCRYPT_MODE);
  printf("key: %016llx\n", key);
  printf("plain: %016llx\n", result);
  printf("encrypted: %016llx\n", e);
  printf("%016llX\n", des(e, key, DECRYPT_MODE));
  return;
}