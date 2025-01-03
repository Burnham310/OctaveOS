# Define source files
SRC = main.c $(wildcard src/*.c) $(wildcard ui/*.c) $(wildcard components/*.c)

# Define object directory and object files
OBJ_DIR = obj
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

# Compiler and flags
CC = clang
INCLUDES = -Iinclude -Iraylib -Ikissfft -Icomponents -Itaglib/bindings/c -Isqlite3
CFLAGS += -g -Wall -Wextra -Wno-switch -Wno-missing-braces 
LDFLAGS = -Lraylib -lraylib -Lkissfft -lkissfft-float -Ltaglib/build/bindings/c -ltag_c -Lsqlite3 -lsqlite3 -lm \
          -Wl,-rpath,raylib -Wl,-rpath,kissfft -Wl,-rpath,taglib/build/bindings/c -Wl,-rpath,sqlite3

# Output directory and executable name
OUT_DIR = bin
EXE_NAME = octaveOS

# Dependency libraries
RAYLIB_SHARED = raylib/libraylib.a
KISSFFT_SHARED = kissfft/libkissfft-float.so
TAGLIB_SHARED = taglib/build/bindings/c/libtag_c.so
SQLITE3_SHARED = sqlite3/libsqlite3.so

# Default target
main: dependencies $(OBJ) | $(OUT_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) $(LDFLAGS) -o $(OUT_DIR)/$(EXE_NAME)

dependencies: $(RAYLIB_SHARED) $(KISSFFT_SHARED) $(TAGLIB_SHARED) $(SQLITE3_SHARED)

$(RAYLIB_SHARED):
	cd raylib && make SHARED=1

$(KISSFFT_SHARED):
	cd kissfft && make

$(TAGLIB_SHARED):
	cd taglib && cmake -B build -DBUILD_SHARED_LIBS=ON -DBUILD_BINDINGS=ON -DCMAKE_BUILD_TYPE=Release
	cd taglib && cmake --build build --config Release

$(SQLITE3_SHARED):
	cd sqlite3 && \
	gcc -shared -fPIC -O2 -o libsqlite3.so sqlite3.c \
	-DSQLITE_THREADSAFE=0 \
	-DSQLITE_DEFAULT_MEMSTATUS=0 \
	-DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1 \
	-DSQLITE_LIKE_DOESNT_MATCH_BLOBS \
	-DSQLITE_MAX_EXPR_DEPTH=0 \
	-DSQLITE_OMIT_DECLTYPE \
	-DSQLITE_OMIT_DEPRECATED \
	-DSQLITE_OMIT_SHARED_CACHE

test: 
	$(CC) $(CFLAGS) -DTEST $(INCLUDES) $(SRC) $(wildcard tests/*.c) $(LDFLAGS) -o $(OUT_DIR)/test
	$(OUT_DIR)/test

tagparse:
	clang -Wall -Wextra -Wno-switch -Wno-missing-braces -Iinclude -Itaglib/bindings/c -Isqlite3 -Ltaglib/build/bindings/c -ltag_c -Lsqlite3 -lsqlite3 -Wl,-rpath,taglib/build/bindings/c -Wl,-rpath,sqlite3 src/tagparse.c src/utils.c src/metalib.c -o bin/test

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
