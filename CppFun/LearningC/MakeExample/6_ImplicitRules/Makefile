#The important variables used by implicit rules are:

#   CC: Program for compiling C programs; default cc
#   CXX: Program for compiling C++ programs; default g++
#   SCFLAGS: Extra flags to give to the C compiler
#   CXXFLAGS: Extra flags to give to the C++ compiler
#   CPPFLAGS: Extra flags to give to the C preprocessor
#   LDFLAGS: Extra flags to give to compilers when they are
#											 supposed to invoke the linker


CC = gcc
CFLAGS = -g # Turns on debug info.

# Implicit rule #1: blah is built via the C linker implicit rule
# Implicit rule #2: blah.o is built via the C compilation implicit
#																			rule, because blah.c exists

# blah.exe: blah.o - sees blah.o as a pre-req, looks for blah.c implicitly.
#																				If blah.c exists, build blah.o.
blah.exe: blah.o

# c file
blah.c:
	echo "int main() { return 0; }" > blah.c

# clean - removes all blah* files.
clean:
	rm -f blah*


# More info on Makefile conventions for CFLAGS: https://www.gnu.org/software/make/manual/html_node/Command-Variables.html
