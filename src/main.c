#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "des.h"

void help(int argc, char** argv);
void keygen(int argc, char** argv);
void encrypt(int argc, char** argv);
void decrypt(int argc, char** argv);

void help(int argc, char** argv) {
  fprintf(stderr, "DES Encryption and Decryption\n");
  fprintf(stderr, "Usage: %s [command] [arguments...]\n", argv[0]);
  fprintf(stderr, "Commands:\n");
  fprintf(stderr, "\tkeygen [key filename]\n");
  fprintf(stderr,
          "\tencrypt [key filename] [plain filename] [encrypted filename]\n");
  fprintf(
      stderr,
      "\tdecrypt [key filename] [encrypted filename] [decrypted filename]\n");
  exit(-1);
}

void keygen(int argc, char** argv) {
  FILE* output_file = fopen(argv[2], "wb+");
  if (output_file == NULL) {
    fprintf(stderr, "Cannot open output file.\n");
    exit(-1);
  }
  char key_bits[8];
  uint64_t key = generate_des_key();
  to_bit(key, key_bits);
  fwrite(key_bits, sizeof(uint8_t), 8, output_file);
  fclose(output_file);
}

void encrypt(int argc, char** argv) {
  des(ENCRYPT_MODE, argv[2], argv[3], argv[4]);
}

void decrypt(int argc, char** argv) {
  des(DECRYPT_MODE, argv[2], argv[3], argv[4]);
}

int main(int argc, char** argv) {
  if (argc < 2) help(argc, argv);
  if (strcmp(argv[1], "keygen") == 0) {
    if (argc != 3) help(argc, argv);
    keygen(argc, argv);
    return 0;
  }
  if (strcmp(argv[1], "encrypt") == 0) {
    if (argc != 5) help(argc, argv);
    encrypt(argc, argv);
    return 0;
  }
  if (strcmp(argv[1], "decrypt") == 0) {
    if (argc != 5) help(argc, argv);
    decrypt(argc, argv);
    return 0;
  }
  help(argc, argv);
  return 0;
}