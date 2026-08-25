# https://makefiletutorial.com/
OBJS := file1.o file2.o
SRCS := file1.c file2.c

# I think "all" is just a convention for this. will always run. It's the first rule listed. It will look for a rule that matches one, two, or three if any of them don't exist.
all: $(SRCS)  $(OBJS)
	@echo "look at first PRE-REQ item in dependency chain: " $< 
	@echo "look at the TARGET item in dependency chain: " $@
	@echo "-- Linking object files --"
	gcc ${OBJS} -o ./build.exe

file1.c:
	touch file1.c
	@echo "int main(void) { return 0; }" > file1.c

@x := This line is bad practice
@y := This line is good practice
file2.c:
	touch file2.c
	echo $x
	echo ${y}

%.o : %.c
	gcc -c $<
# $< will point to the first file in the depedency chain
# $@ will point to the last file in the dependency chain

# only runs if "make clean" is sent to command line because other rules will always run before it because it is at the bottom. If this were placed at the top, it would run every time. 
clean:
	rm -f *.c beegFile *.o 
