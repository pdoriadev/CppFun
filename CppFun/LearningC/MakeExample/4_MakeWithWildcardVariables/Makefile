
sourceFiles := file1.c file2.c file3.c

print: $(wildcard *.c)
	ls -la $^

build: source $(sourceFiles)
	gcc -c $

$(objectFiles): source
	gcc $@ $(sourceFiles)?

# This whole line is the dependency chain.
# source is the target in this dependency chain. 
source: $(sourceFiles)
	printf "Source Files Verified"
	#$@ references the last file in the dependency chain


$(sourceFiles):
	touch $@
