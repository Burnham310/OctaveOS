# Define source files
SRC = main.c $(wildcard src/*.c) $(wildcard ui/*.c) $(wildcard components/*.c)

# Define object directory and object files
OBJ_DIR = obj
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

# Compiler and flags
CC = clang
INCLUDES = -Iinclude -Iraylib -Ikissfft -Icomponents
CFLAGS += -g -Wall -Wextra -Wno-switch -Wno-missing-braces 
LDFLAGS = -Lraylib -lraylib -Lkissfft -lkissfft-float -lm -Wl,-rpath,raylib -Wl,-rpath,kissfft

# Output directory and executable name
OUT_DIR = bin
EXE_NAME = octaveOS

# Build target
main: $(OBJ) | $(OUT_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ $(LDFLAGS) -o $(OUT_DIR)/$(EXE_NAME)

test: 
	$(CC) $(CFLAGS) -DTEST $(INCLUDES) ${SRC} $(wildcard tests/*.c) $(LDFLAGS) -o $(OUT_DIR)/test
	$(OUT_DIR)/test

# Test Random module
%: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $< $(LDFLAGS) -o $(OUT_DIR)/$@
	$(OUT_DIR)/$@

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/src
	mkdir -p $(OBJ_DIR)/ui
	mkdir -p $(OBJ_DIR)/components

# Create output directory
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Compile source files to object files
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OUT_DIR)/$(EXE_NAME)
	rm -rf $(OBJ_DIR)

# Run the executable
run: main
	$(OUT_DIR)/$(EXE_NAME)

.PHONY: all clean run
