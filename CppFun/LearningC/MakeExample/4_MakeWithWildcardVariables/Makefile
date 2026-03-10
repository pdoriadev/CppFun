
sourceFiles := file1.c file2.c file3.c
OBJS = file1.o file2.o file3.o

print: $(wildcard *.c)
	ls -la $^

all: source ${OBJS}
	@echo "-------------------------------------"
	@echo "------- Linking Executable ----------"
gcc build.exe -o ${OBJS}

# For each file with a .c extension, compile an object file with a filename that matches the .c file's  % wildcard stem.  
%.o : %.c
	@echo "-------------------------------------"
	@echo "------- Compiling C Files -----------"
	gcc -c $<

# This whole line is the dependency chain. Source is the target in this dependency chain. 
# For each string in sourceFiles, make a file of the same name
source: $(sourceFiles)
#$@ references the last file in the dependency chain
source: sourceFiles
	touch $@

# Example of a phony target. We don't want a file named clean. We want to remove files based on that. 
clean: 
	rm -f *.c *.o
