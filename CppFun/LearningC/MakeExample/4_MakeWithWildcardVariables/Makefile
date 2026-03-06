
sourceFiles := file1.c file2.c file3.c

print: $(wildcard *.c)
	ls -la $^

build: source $(sourceFiles)
	gcc -c $

$(objectFiles): source
	gcc $@ $(sourceFiles)?


# target (what the rule makes? Or just the nameof the rule?) : pre-req (what the rule needs)
source: $(sourceFiles)
	printf "Source Files Verified"
	#$@ references what the rule makes


$(sourceFiles):
	touch $@
