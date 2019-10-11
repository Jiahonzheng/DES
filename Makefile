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