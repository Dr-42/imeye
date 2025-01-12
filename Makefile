# Makefile for imeye project

# Project Name
PROJECT_NAME := imeye

# Directories
SRC_DIR := src
OBJ_DIR := build/obj
BIN_DIR := build/bin
INC_DIR := $(SRC_DIR)/include

# Source and Header Files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))
HEADER_FILES := $(wildcard $(INC_DIR)/*.h)

# Compiler and Flags
CFLAGS := -Wall -Wextra -I$(INC_DIR)
LDFLAGS := -lm -lGLEW -lglfw -lGL

# Default Target
.PHONY: all
all: $(BIN_DIR)/$(PROJECT_NAME)

# Build Binary
$(BIN_DIR)/$(PROJECT_NAME): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)

# Build Object Files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER_FILES) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure Build Directories Exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean Build
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Help Command
.PHONY: help
help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all      Build the project (default)"
	@echo "  clean    Remove all build files"
	@echo "  help     Display this help message"
