# Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g3 -MMD -MP -I$(SRC_DIR)
LDFLAGS = 

# Directories
SRC_DIR = source
OBJ_DIR = build
OUT_DIR = output
TEST_DIR = test
TEST_BIN_DIR = $(OUT_DIR)/test

# Target executable
TARGET = $(OUT_DIR)/pinch

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

# Test configuration
APP_MAIN_OBJ = $(OBJ_DIR)/pinch.o
SHARED_OBJS = $(filter-out $(APP_MAIN_OBJ), $(OBJS))
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(patsubst $(TEST_DIR)/%.c,$(TEST_BIN_DIR)/%,$(TEST_SRCS))

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS) | $(OUT_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# "make test" will compile and run all tests
test: $(TEST_BINS)
	@echo "--------------------------------------"
	@echo "Running Tests...\n"
	@for test_bin in $(TEST_BINS); do \
		echo "Executing $$test_bin..."; \
		./$$test_bin; \
		echo ""; \
	done
	@echo "All tests completed."

# Rule to compile a specific test executable
# It links the test source ($<) with the shared objects ($(SHARED_OBJS))
$(TEST_BIN_DIR)/%: $(TEST_DIR)/%.c $(SHARED_OBJS) | $(TEST_BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $< $(SHARED_OBJS) -o $@

# Create test bin directory
$(TEST_BIN_DIR):
	mkdir -p $@

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
