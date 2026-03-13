SRCS := file1.c file2.c file3.c 
OBJS = file1.o file2.o file3.o

print: $(wildcard *.c)
	ls -la $^


# This whole line is the dependency chain. all is the target in this dependency chain. 
all: ${SRCS} ${OBJS}
	@# @command- make usually outputs lines to the console before the line is executed. The '@' symbol stops that. This makes sense for print statements, echo commands, etc. The silent flag '-s' or '--silent' prevents all default echoing, as if all lines started with @. https://www.gnu.org/software/make/manual/html_node/Echoing.htm 
	@echo "-------------------------------------"
	@echo "------- Linking Executable ----------"
	gcc ${OBJS} -o ./build.exe

# For each file with a .c extension, compile an object file with a filename that matches the .c file's  % wildcard stem.  
%.o : %.c
	@echo "-------------------------------------"
	@echo "------- Compiling C Files -----------"
	gcc -c $<

#@ references the last file in the dependency chain
file1.c:
	touch $@
	echo "int main(void) { return 0; }" > file1.c

file2.c:
	touch $@

file3.c:
	touch $@

# Example of a phony target. We don't want a file named clean. We want to remove files based on that. 
clean: 
	rm -f *.c *.o
