# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -Iinclude

# Project structure
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Project files
TARGET = mp4_parser
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
HEADERS = $(wildcard $(INC_DIR)/*.h)

# Set default goal
.DEFAULT_GOAL := all

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Compilation with automatic dependency generation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Include dependency files
-include $(OBJS:.o=.d)

# Run target
run: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET) $(OBJ_DIR)/*.d
	rmdir $(OBJ_DIR) 2>/dev/null || true

# Format source files
format:
	find $(SRC_DIR) $(INC_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i -style=file

# Phony targets
.PHONY: all clean run format

# Create object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
