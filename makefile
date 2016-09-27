# Makefile Usage:
# make all - makes three driver executables
# make mem_mod[1-3] - makes a program executable with the name mem_mod[1-3]
# make clean - removes object files, temporary files, and executables

CC=gcc
CFLAGS=-m32 

all: mem_mod1 mem_mod2 mem_mod3

mem_mod1: mem_mod1.o memlayout.o
	$(CC) mem_mod1.o memlayout.o $(CFLAGS) -o mem_mod1 
mem_mod2: mem_mod2.o memlayout.o
	$(CC) mem_mod2.o memlayout.o $(CFLAGS) -o mem_mod2
mem_mod3: mem_mod3.o memlayout.o
	$(CC) mem_mod3.o memlayout.o $(CFLAGS) -o mem_mod3

memlayout.o: memlayout.c 
	$(CC) memlayout.c -c  $(CFLAGS)

mem_mod1.o: mem_mod1.c
	$(CC) mem_mod1.c -c $(CFLAGS)
mem_mod2.o: mem_mod2.c
	$(CC) mem_mod2.c -c $(CFLAGS)
mem_mod3.o: mem_mod3.c
	$(CC) mem_mod3.c -c $(CFLAGS)

clean: 
	rm -f *.o *~ mem_mod1 mem_mod2 mem_mod3
