# Default target
main: dependencies $(OBJ) | $(OUT_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) $(LDFLAGS) -o $(OUT_DIR)/$(EXE_NAME)

# Define source files
SRC = main.c $(wildcard src/*.c) $(wildcard ui/*.c) $(wildcard components/*.c)

# Define object directory and object files
OBJ_DIR = obj
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

# Compiler and flags
CC = clang
INCLUDES = -Iinclude -Iraylib -Ikissfft -Icomponents -Itaglib/bindings/c
CFLAGS += -g -Wall -Wextra -Wno-switch -Wno-missing-braces 
LDFLAGS = -Lraylib -lraylib -Lkissfft -lkissfft-float -Ltaglib/bindings/c -ltag_c -lm \
          -Wl,-rpath,raylib -Wl,-rpath,kissfft -Wl,-rpath,taglib/bindings/c

# Output directory and executable name
OUT_DIR = bin
EXE_NAME = octaveOS

# Dependency libraries
RAYLIB_SHARED = raylib/libraylib.a
KISSFFT_SHARED = kissfft/libkissfft-float.so
TAGLIB_SHARED = taglib/build/bindings/c/libtag_c.so

# Mark dependencies as a phony target
.PHONY: dependencies
dependencies: $(RAYLIB_SHARED) $(KISSFFT_SHARED) $(TAGLIB_SHARED)

$(RAYLIB_SHARED):
	cd raylib && make SHARED=1

$(KISSFFT_SHARED):
	cd kissfft && make

$(TAGLIB_SHARED):
	cd taglib && cmake -B build -DBUILD_SHARED_LIBS=ON -DBUILD_BINDINGS=ON -DCMAKE_BUILD_TYPE=Release
	cd taglib && cmake --build build --config Release

test: 
	$(CC) $(CFLAGS) -DTEST $(INCLUDES) $(SRC) $(wildcard tests/*.c) $(LDFLAGS) -o $(OUT_DIR)/test
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
	cd raylib && make clean
	cd kissfft && make clean
	rm -rf taglib/build

# Run the executable
run: main
	$(OUT_DIR)/$(EXE_NAME)

.PHONY: all clean run
