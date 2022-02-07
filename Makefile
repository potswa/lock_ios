lib=liblock_ios
flags= -Wall -Werror 
libflags= -lssp_nonshared
src=src/lock_ios.cpp
CC=g++
include=-I./
all: $(lib) 
clean: $(lib)
	rm -f $(^).o $^

$(lib):
	$(CC) -c -fstack-protector $(flags) $(include) $(src) -o $(@).o
mk_shared:
	ld  -A x86_64 $(lib).o $(libcpp) $(libssp)

.PHONY: $(src)
$(src): $(lib)
	$(CC) -fstack-protector $@ $(flags) $(include) $(libflags) -o  $^

