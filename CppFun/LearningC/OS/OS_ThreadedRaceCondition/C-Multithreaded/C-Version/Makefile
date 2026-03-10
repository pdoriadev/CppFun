# Use gcc for linking and compilation instead of cc
CC = gcc
# Use debug flag
CFLAGS = -g

# Implicit rule #1: object files are built via the C linker implicit rule
# Implicit rule #2: *.o files built via the C compilation implicit rule, because>
banking.exe : $(wildcard *.o)
	$(CC) -o main.exe build/*.o

build/$(wildcard *.o) : $(wildcard *.c)


clean:
 rm *.o *.exe

