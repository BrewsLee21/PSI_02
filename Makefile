CC = gcc
CFLAGS = -Wall -Wextra -g -I./lib/libcrc/include
LDFLAGS = -L./lib/libcrc/lib -lcrc

# 1. Define the object directory
OBJDIR = obj

OBJ_COMMON = $(OBJDIR)/utils.o $(OBJDIR)/network.o
OBJ_SENDER = $(OBJDIR)/sender.o $(OBJ_COMMON)
OBJ_RECEIVER = $(OBJDIR)/receiver.o $(OBJ_COMMON)

all: $(OBJDIR) sender receiver

# 2. Rule to create the object directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

sender: $(OBJ_SENDER)
	$(CC) $(CFLAGS) -o sender $(OBJ_SENDER) $(LDFLAGS)

receiver: $(OBJ_RECEIVER)
	$(CC) $(CFLAGS) -o receiver $(OBJ_RECEIVER) $(LDFLAGS)

# 3. Pattern rule: Compile any .c file into the $(OBJDIR)
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) sender receiver
