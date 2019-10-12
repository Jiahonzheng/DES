CC=gcc
INC_DIR=include
SRC_DIR=src
BIN_DIR=bin
BUILD_DIR=build
OBJ_DIR=obj

SOURCE_FILES=$(shell find $(SRC_DIR) -name '*.c')
OBJS=$(patsubst $(SOURCE_FILES)/%.c,$(BUILD_DIR)/%.o,$(SOURCE_FILES))

$(BIN_DIR)/des: $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -Iinclude -w $^ -o $@ -lm

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -Iinclude -w -c -o $@ $<

clean:
	@rm -rf bin/*

test:
	@echo "Clean the previous binary build."
	@make clean

	@echo "\nBuild the new binary."
	@make

	@echo "\nGenerate a key."
	@./bin/des keygen key
	@echo "The hexdump of the key:"
	@hexdump key

	@echo "\nGenerate a plain file."
	@hexdump key > plain
	@echo "The hexdump of the plain file:"
	@hexdump plain

	@echo "\nEncrypt the plain file with the key."
	@./bin/des encrypt key plain encrypted
	@echo "The hexdump of the encrypted file:"
	@hexdump encrypted

	@echo "\nDecrypt the encrypted file with the key."
	@./bin/des decrypt key encrypted decrypted
	@echo "The hexdump of the decrypted file:"
	@hexdump decrypted

	@echo "\nThe difference between the plain file and the decrypted file:"
	@diff plain decrypted

	@echo "\nClean the testing files."
	@rm -f key plain encrypted decrypted
