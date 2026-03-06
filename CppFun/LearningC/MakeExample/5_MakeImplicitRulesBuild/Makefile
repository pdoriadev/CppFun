CC = gcc #Flag for implicit rules
CFLAGS = -g # Flag for implicit rules. Turn on debug info.

objects := file1.o file2.o file3.o

# Implicit rule #1: object files are built via the C linker implicit rule
# Implicit rule #2: blah.o is built via the C compilation implicit rule, because blah.c exists
build: build/ $(objects)
	$(CC) -o build/build.exe build/*.o

build/:
	mkdir build/

file1.o: file1.c
	make source
	$(CC) -c file1.c -o build/file1.o

file2.o: file2.c
	make source
	$(CC) -c file2.c -o build/file2.o

file3.o: file3.c
	make source
	$(CC) -c file3.c -o build/file3.o

sourceFiles := file1.c file2.c file3.c
source: $(sourceFiles)

$(sourceFiles):
	touch $@

clean:
	rm -f *.o *.c

