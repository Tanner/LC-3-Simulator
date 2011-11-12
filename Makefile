###########################################
# Author: Brandon Whitehead
# Editor: Tanner Smith
###########################################

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Werror
OPTFLAG = -O2
DEBUGFLAG = -g

PROGRAM = lc3sim

all: build-release
run: build-release
	lc3as test.asm
	./$(PROGRAM) test.obj

build-release: CFLAGS += $(OPTFLAG)
build-release: $(PROGRAM)

build-debug: CFLAGS += $(DEBUGFLAG)
build-debug: $(PROGRAM)

debug: clean build-debug
	gdb ./$(PROGRAM)

$(PROGRAM): lc3sim.o lc3.o
	gcc lc3sim.o lc3.o -o $(PROGRAM)
	
lc3sim.o : lc3sim.c lc3.h
	gcc $(CFLAGS) -c lc3sim.c
	
lc3.o : lc3.h lc3.c
	gcc $(CFLAGS) -c lc3.c

clean:
	rm -f *.o $(PROGRAM)

.PHONY: all build clean
