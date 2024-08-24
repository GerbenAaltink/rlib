# RLIB

Collection of frequently used C functions by me. 

It contains 
 - An advanced benchmark system.
 - An alternative to printf supporting printing of time between messages for e.g. benchmarking.
   and colors.
 - Super fast tree map (much faster than hash table).
 - Stdout redirection.
 - Terminal progress bar.
 - Multi purpose lexer.
 - Custom malloc for counting allocations and free's.
 - Simple test library that works like assert that also checks memory if rmalloc is used and
   creates summary and provides exit code.
 - Several time functions supporting nano seconds.
 - Several math functions for if not available by std (on termux for example).
 - Arena blazing fast memory.

## ENVIRONMENT VARIABLES

###  Disabling color
Set env RDISABLE_COLORS = 1 to disable color on stdout. stderr will still have colors for gui.

## Compiler and Flags
- **`CC = gcc`**: Specifies the C compiler (GCC).
- **`CFLAGS = -Wall -Wextra -Ofast`**: Compiler flags:
  - `-Wall`: Enable all warnings.
  - `-Wextra`: Enable extra warnings.
  - `-Ofast`: Optimize for speed.
- **`LDFLAGS = -lm`**: Linker flags to link with the math library.

## Main Targets

### `all:`
Runs the following steps in sequence:
- Testing (`test_*` targets).
- Formatting code (`format_all`).
- Building the project (`build`).
- Installing executables (`install`).

### `format_all:`
Runs `clang-format` to format all `.c`, `.h`, and `.cpp` files in the directory.

### `build:`
Builds the `rlib` shared library (`librlib.so`) and an executable `rlibso` that uses this library:
- Compiles `rlib.c` into `librlib.so`.
- Compiles `rlibso.c` linking it with `librlib.so`.
- Runs the `rlibso` executable.

### `install:`
Installs the `rmerge` and `clean` executables to `/usr/bin` using `sudo`.

## Testing Targets (`test_*`)

Each `test_*` target follows these steps:

### `build_*:`
Compiles a specific `.c` file into an executable in the `./build` directory.

### `run_*:`
Runs the corresponding executable.

Example for `rlexer`:
- **`build_rlexer:`** Compiles `rlexer.c` to `./build/rlexer`.
- **`run_rlexer:`** Runs `./build/rlexer`.

## Backup

### `backup:`
Compresses the source files, Makefile, and additional project files into a `rlib.rzip` archive.

## Special Cases

### `test_rbench_cpp:`
Builds and runs a C++ benchmark program:
- Compiles `rbench.cpp` along with other necessary files.
- Runs the resulting executable.

### `test_yurii_cpp:`
Merges and formats a C++ file, then builds and runs it:
- Merges `rbench.cpp` into `yurii_hashmap.cpp`.
- Compiles and runs `yurii_hashmap.cpp`.

## Other Targets

### `format_rlib_c:`
Specifically formats the `rlib.c` file.

### `build_*:`
Targets that compile source files into executables.

### `run_*:`
Targets that run the compiled executables.