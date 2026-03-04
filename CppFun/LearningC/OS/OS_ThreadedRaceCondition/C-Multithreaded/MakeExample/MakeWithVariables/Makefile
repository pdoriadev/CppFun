# will always run. It's the first rule listed. It will look for a rule that matches one, two, or three if any of them don't exist.
all: one two three
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

# only runs if "make clean" is sent to command line. Why it's placed at the bottom? I think it's a specific make keyword?
clean:
	rm -f file1.c file2.c beegFile
