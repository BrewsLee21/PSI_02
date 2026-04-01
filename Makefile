CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lssl -lcrypto -lz

OBJ_DIR = obj

# Object files
OBJ_COMMON = $(OBJ_DIR)/network.o $(OBJ_DIR)/utils.o
OBJ_SENDER = $(OBJ_DIR)/sender.o $(OBJ_COMMON)
OBJ_RECEIVER = $(OBJ_DIR)/receiver.o $(OBJ_COMMON)

all: sender receiver

sender: $(OBJ_SENDER)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

receiver: $(OBJ_RECEIVER)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile .c -> obj/.o
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) sender receiver

.PHONY: all clean
