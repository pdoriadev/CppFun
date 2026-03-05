# https://makefiletutorial.com/
objectFiles := f1.o f2.o
sourceFiles := file1.c file2.c

# I think "all" is just a convention for this. will always run. It's the first rule listed. It will look for a rule that matches one, two, or three if any of them don't exist.
all: one two three beegFile $(objectFiles)
	echo "all command pre-reqs were fulfilled"

one:
	touch one
two:
	touch two
three:
	touch three

beegFile: $(sourceFiles)
	echo "look at this variable: " $(sourceFiles)
	touch beegFile

file1.c:
	touch file1.c

x := This line is bad practice
y := This line is good practice
file2.c:
	touch file2.c
	echo $x
	echo ${y}

$(objectFiles):
	echo $@

# only runs if "make clean" is sent to command line because other rules will always run before it because it is at the bottom. If this were placed at the top, it would run every time. 
clean:
	rm -f file1.c file2.c beegFile f1.o f2.o
