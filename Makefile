# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -Ofast -std=c2x -lm -lrt -pthread -fPIC 
LDFLAGS = -lm
 
# -D_POSIX_C_SOURCE=199309L 
all: clear test_rbuffer test_rliza test_rnet test_rmalloc test_rtemp test_rjson test_rrex4 test_rstring_list test_rhttp test_rtime test_arena test_rtree test_rstring test_rlexer test_rrex3 test_rio test_rhashtable test_rkeytable test_rterminal test_rmerge run_rmerge format_all build format_all test_rlib 




format_all: format_rlib_h
	clang-format *.c *.h *.cpp -i --sort-includes=false

format_rlib_h:
	clang-format rlib.h build/rlib.h -i --sort-includes --verbose

clear:
	-@rm -rf build
	@mkdir build
	@echo "Build directory emptied"

rebuild: clear all

test_rbuffer: build_rbuffer run_rbuffer
build_rbuffer:
	$(CC) $(CFLAGS)  rbuffer.c -o ./build/rbuffer	
run_rbuffer:
	./build/rbuffer test

test_rliza: build_rliza run_rliza
build_rliza:
	$(CC) $(CFLAGS)  rliza.c -o ./build/rliza	
run_rliza:
	./build/rliza test

test_rnet: build_rnet run_rnet
build_rnet:
	$(CC) $(CFLAGS)  rnet.c -o ./build/rnet	
run_rnet:
	./build/rnet test

test_rtemp: build_rtemp run_rtemp
build_rtemp:
	$(CC) $(CFLAGS) rtemp.c -o ./build/rtemp	
run_rtemp:
	./build/rtemp

test_rjson: build_rjson run_rjson
build_rjson:
	$(CC) $(CFLAGS) rjson.c -o ./build/rjson	
run_rjson:
	./build/rjson


test_rstring_list: build_rstring_list run_rstring_list
build_rstring_list:
	$(CC) $(CFLAGS) rstring_list.c -o ./build/rstring_list	
run_rstring_list:
	./build/rstring_list

test_rhttp: build_rhttp run_rhttp
build_rhttp:
	$(CC) $(CFLAGS) rhttp.c -o ./build/rhttp	
run_rhttp:
	./build/rhttp

test_rautocomplete: build_rautocomplete run_rautocomplete
build_rautocomplete:
	$(CC) $(CFLAGS) rautocomplete.c -o ./build/rautocomplete	
run_rautocomplete:
	./build/rautocomplete

test_rlexer: build_rlexer run_rlexer
build_rlexer:
	$(CC) $(CFLAGS) rlexer.c -o ./build/rlexer
run_rlexer:
	./build/rlexer


test_rrex4: build_rrex4 run_rrex4
build_rrex4:
	$(CC) $(CFLAGS) rrex4.c -o ./build/rrex4
run_rrex4:
	./build/rrex4

test_rrex3: build_rrex3 run_rrex3
build_rrex3:
	$(CC) $(CFLAGS) rrex3.c -o ./build/rrex3
run_rrex3:
	./build/rrex3

test_rio: build_rio run_rio
build_rio:
	$(CC) $(CFLAGS) rio.c -o ./build/rio
run_rio:
	./build/rio



test_rterm: build_rterm run_rterm
build_rterm:
	$(CC) $(CFLAGS) rterm.c -o ./build/rterm
run_rterm:
	./build/rterm

test_rterminal: build_rterminal run_rterminal
build_rterminal:
	$(CC) $(CFLAGS) rterminal.c -o ./build/rterminal
run_rterminal:
	./build/rterminal

test_rmerge: build_rmerge run_rmerge
build_rmerge:
	$(CC) $(CFLAGS) rmerge.c -o ./build/rmerge
run_rmerge:
	./build/rmerge _rlib.h > ./build/rlib.h
	cp ./build/rlib.h ./rlib.h

test_rprint: build_rprint run_rprint
build_rprint:
	$(CC) $(CFLAGS) rprint.c -o ./build/rprint
run_rprint:
	./build/rprint

test_rstring: build_rstring run_rstring
build_rstring:
	$(CC) $(CFLAGS) rstring.c -o ./build/rstring
run_rstring:
	./build/rstring

test_rbench: build_rbench run_rbench
build_rbench:
	$(CC) $(CFLAGS) rbench.c -o ./build/rbench
run_rbench:
	./build/rbench

test_rbench_cpp: build_rbench_cpp run_rbench_cpp
build_rbench_cpp:
	g++ $(CFLAGS) ./C-Data-Structs/src/primes.c rbench.cpp  -o ./build/rbench.cpp -I./C-Data-Structs/include
run_rbench_cpp:
	./build/rbench.cpp

test_yurii_cpp: format_all build_yurii_cpp run_yurii_cpp
build_yurii_cpp:
	rmerge rbench.cpp > yurii_hashmap.cpp
	g++ C-Data-Structs/src/primes.c yurii_hashmap.cpp -o ./build/yurii_hashmap.cpp -IC-Data-Structs/include
run_yurii_cpp:
	./build/yurii_hashmap.cpp

test_rmalloc: build_rmalloc run_rmalloc
build_rmalloc:
	$(CC) $(CFLAGS) rmalloc.c -o ./build/rmalloc 
run_rmalloc:
	./build/rmalloc 

test_rtime: build_rtime run_rtime
build_rtime:
	$(CC) $(CFLAGS) rtime.c -o ./build/rtime
run_rtime:
	./build/rtime

test_arena: build_arena run_arena
build_arena:
	$(CC) $(CFLAGS) arena.c -o ./build/arena
run_arena:
	./build/arena

test_rlib: build_rlib run_rlib
build_rlib:
	$(CC) $(CFLAGS) rlib.c -o ./build/rlib
run_rlib:
	./build/rlib test

test_rtree: build_rtree run_rtree
build_rtree:
	$(CC) $(CFLAGS) rtree.c -o ./build/rtree
run_rtree:
	./build/rtree

test_rhashtable: build_rhashtable run_rhashtable
build_rhashtable:
	$(CC) $(CFLAGS) rhashtable.c -o ./build/rhashtable
run_rhashtable:
	./build/rhashtable

test_rkeytable: build_rkeytable run_rkeytable
build_rkeytable:
	$(CC) $(CFLAGS) rkeytable.c -o ./build/rkeytable
run_rkeytable:
	./build/rkeytable

build: format_rlib_h
	cp ./clean build/clean
	cp rlib.h rlibrlibso.c
	@gcc rlibrlibso.c $(CFLAGS) -shared -o ./build/librlib.so
	@echo "Built a new rlib.so"
	@gcc rlibso.c -L./build -Wl,-rpath=. -lrlib -o  ./build/rlibso ${CFLAGS}
	@cd ./build && ./rlibso
	@echo "Build succesful"

install:
	sudo cp ./build/rlib.h /usr/include/rlib.h
	sudo cp ./build/rmerge /usr/local/bin/rmerge
	sudo cp ./build/clean /usr/local/bin/clean
	sudo cp ./build/rlib /usr/local/bin/rlib

publish:
	brz add 
	brz commit 
	brz push lp:rlib
