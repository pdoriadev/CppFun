# will always run. It's the first rule listed. It will look for a rule that matches one, two, or three if any of them don't exist.
all: one two three beegFile
	echo "all command pre-reqs were fulfilled"

one:
	touch one
two:
	touch two
three:
	touch three

files := file1.c file2.c
beegFile: $(files)
	echo "look at this variable: " $(files)
	touch beegFile

file1.c:
	touch file1.c

x := This line is bad practice
y := This line is good practice
file2.c:
	touch file2.c
	echo ${y}

	echo $x

# only runs if "make clean" is sent to command line because other rules will always run before it because it is at the bottom. If this were placed at the top, it would run every time. 
clean:
	rm -f file1.c file2.c beegFile
