CC=gcc
CFLAGS=-c -O2 -Wall -g

all: lab03 testcase

lab03: sim.o lab03.o
	$(CC) sim.o lab03.o -o lab03

testcase: testcase.o
	$(CC) sim.o testcase.o -o testcase

mmu.o: mmu.c
	$(CC) $(CFLAGS) sim.c

lab03.o: lab03.c
	$(CC) $(CFLAGS) lab03.c

clean:
	/bin/rm -f lab03 testcase *.o *.log

run:
	./lab03 1 512 512 200 1234

test:
	./testharness.sh


