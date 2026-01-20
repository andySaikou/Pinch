# Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g3 -MMD -MP
LDFLAGS = 

# Directories
SRC_DIR = source
OBJ_DIR = build
OUT_DIR = output

# Target executable
TARGET = $(OUT_DIR)/pinch

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS) | $(OUT_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Create directories if they don't exist
$(OBJ_DIR):
	mkdir -p $@

$(OUT_DIR):
	mkdir -p $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)

# Include the dependency files
-include $(DEPS)

# Phony targets (not actual files)
.PHONY: all clean
