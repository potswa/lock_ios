lib=liblock_ios
flags= -Wall -Werror 
libflags= -lssp 
src=src/lock_ios.cpp
CC=g++
include=-I./

all: $(lib) mk_shared
$(lib):
	$(CC) -c -fstack-protector $(flags) $(include) $(src) -o $(@).o
mk_shared:
	ld  -A x86_64 $(lib).o $(libcpp) $(libssp)
clean:
	rm -f $(lib).o

