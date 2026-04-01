CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lssl -lcrypto -lz

OBJ_DIR = obj

# Common object files
OBJ_COMMON = $(OBJ_DIR)/common_net.o $(OBJ_DIR)/utils.o

# Sender objects
OBJ_SENDER = $(OBJ_DIR)/sender.o $(OBJ_DIR)/sender_net.o $(OBJ_COMMON)

# Receiver objects
OBJ_RECEIVER = $(OBJ_DIR)/receiver.o $(OBJ_DIR)/receiver_net.o $(OBJ_COMMON)

# Default target
all: sender receiver

# Executables
sender: $(OBJ_SENDER)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

receiver: $(OBJ_RECEIVER)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile .c -> obj/.o
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJ_DIR) sender receiver

.PHONY: all clean
