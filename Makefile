# Compiler
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -Ofast
LDFLAGS = -lm

# Directories
BUILD_DIR = build
SRC_DIR = .

# Source files and their corresponding object files
SOURCES = rlexer.c rterminal.c rmerge.c rprint.c rstring.c rbench.c rbench.cpp yurii_hashmap.cpp rmalloc.c rtime.c arena.c rtree.c rhashtable.c rkeytable.c
OBJECTS = $(SOURCES:.c=.o) $(SOURCES:.cpp=.o)

# Default target
all: format_all $(BUILD_DIR)/rlib.so $(BUILD_DIR)/rlibso test_rprint test_rmalloc test_rtime test_arena test_rtree test_rstring test_rlexer test_rhashtable test_rkeytable test_rterminal test_rmerge

# Formatting targets
format_all: format_rlib_h
	clang-format *.c *.h *.cpp -i --sort-includes=false

format_rlib_h:
	clang-format rlib.h build/rlib.h -i --sort-includes --verbose

# Build shared library
$(BUILD_DIR)/rlib.so: rlib.c
	$(CC) $(CFLAGS) -fPIC -shared rlib.c -o $@
	$(CC) $(GCFLAGS) rlib.c -fPIC -shared -o ./build/librlib.so $@

# Build executable
$(BUILD_DIR)/rlibso: rlibso.c $(BUILD_DIR)/rlib.so
	$(CC) $(CFLAGS) rlibso.c -L$(BUILD_DIR) -Wl,-rpath=$(BUILD_DIR) -lrlib -o $@

# Compile sources into objects
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

# Test targets
test_rlexer: $(BUILD_DIR)/rlexer
	$(BUILD_DIR)/rlexer

$(BUILD_DIR)/rlexer: rlexer.o
	$(CC) $(CFLAGS) $< -o $@

test_rterminal: $(BUILD_DIR)/rterminal
	$(BUILD_DIR)/rterminal

$(BUILD_DIR)/rterminal: rterminal.o
	$(CC) $(CFLAGS) $< -o $@

test_rmerge: $(BUILD_DIR)/rmerge
	$(BUILD_DIR)/rmerge _rlib.h > $(BUILD_DIR)/rlib.h
	cp $(BUILD_DIR)/rlib.h ./rlib.h
	cp $(BUILD_DIR)/rlib.h ./rlib.c

$(BUILD_DIR)/rmerge: rmerge.o
	$(CC) $(CFLAGS) $< -o $@

test_rprint: $(BUILD_DIR)/rprint
	$(BUILD_DIR)/rprint

$(BUILD_DIR)/rprint: rprint.o
	$(CC) $(CFLAGS) $< -o $@

test_rstring: $(BUILD_DIR)/rstring
	$(BUILD_DIR)/rstring

$(BUILD_DIR)/rstring: rstring.o
	$(CC) $(CFLAGS) $< -o $@

test_rbench: $(BUILD_DIR)/rbench
	$(BUILD_DIR)/rbench

$(BUILD_DIR)/rbench: rbench.o
	$(CC) $(CFLAGS) $< -o $@

test_rbench_cpp: $(BUILD_DIR)/rbench_cpp
	$(BUILD_DIR)/rbench_cpp

$(BUILD_DIR)/rbench_cpp: rbench.cpp
	g++ $(CFLAGS) rbench.cpp -o $@ -I./C-Data-Structs/include

test_yurii_cpp: format_all $(BUILD_DIR)/yurii_hashmap_cpp
	$(BUILD_DIR)/yurii_hashmap_cpp

$(BUILD_DIR)/yurii_hashmap_cpp: yurii_hashmap.cpp
	rmerge rbench.cpp > yurii_hashmap.cpp
	g++ C-Data-Structs/src/primes.c yurii_hashmap.cpp -o $@ -IC-Data-Structs/include

test_rmalloc: $(BUILD_DIR)/rmalloc
	$(BUILD_DIR)/rmalloc

$(BUILD_DIR)/rmalloc: rmalloc.o
	$(CC) $(CFLAGS) $< -o $@

test_rtime: $(BUILD_DIR)/rtime
	$(BUILD_DIR)/rtime

$(BUILD_DIR)/rtime: rtime.o
	$(CC) $(CFLAGS) $< -o $@

test_arena: $(BUILD_DIR)/arena
	$(BUILD_DIR)/arena

$(BUILD_DIR)/arena: arena.o
	$(CC) $(CFLAGS) $< -o $@

test_rtree: $(BUILD_DIR)/rtree
	$(BUILD_DIR)/rtree

$(BUILD_DIR)/rtree: rtree.o
	$(CC) $(CFLAGS) $< -o $@

test_rhashtable: $(BUILD_DIR)/rhashtable
	$(BUILD_DIR)/rhashtable

$(BUILD_DIR)/rhashtable: rhashtable.o
	$(CC) $(CFLAGS) $< -o $@

test_rkeytable: $(BUILD_DIR)/rkeytable
	$(BUILD_DIR)/rkeytable

$(BUILD_DIR)/rkeytable: rkeytable.o
	$(CC) $(CFLAGS) $< -o $@

# Install target
install:
	sudo cp $(BUILD_DIR)/rmerge /usr/bin/rmerge
	sudo cp $(BUILD_DIR)/clean /usr/bin/clean
	sudo cp $(BUILD_DIR)/rlib.h /usr/include/rlib.h

# Publish target
publish:
	brz add 
	brz commit 
	brz push lp:rlib

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.so $(BUILD_DIR)/*.cpp $(BUILD_DIR)/rmerge $(BUILD_DIR)/rprint $(BUILD_DIR)/rstring $(BUILD_DIR)/rbench $(BUILD_DIR)/rbench_cpp $(BUILD_DIR)/yurii_hashmap_cpp $(BUILD_DIR)/rmalloc $(BUILD_DIR)/rtime $(BUILD_DIR)/arena $(BUILD_DIR)/rtree $(BUILD_DIR)/rhashtable $(BUILD_DIR)/rkeytable
