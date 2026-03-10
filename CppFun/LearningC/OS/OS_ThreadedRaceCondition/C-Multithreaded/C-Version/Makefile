# Use gcc for linking and compilation instead of cc
CC = gcc
# Use debug flag
CFLAGS = -g

# Implicit rule #1: object files are built via the C linker implicit rule
# Implicit rule #2: *.o files built via the C compilation implicit rule, because>


all:
	make objs
#	$(CC) build -o $(OBJS)

SOURCE_DIR = /SOURCE/
OBJS_DIR = /OBJS/

SRC = $(patsubst %.c, %.o$(SOURCE_DIR)$(wildcard *.c))
OBJS = $(patsubst %.c, %.o, $(SRC))
objs : $(OBJS_DIR)$(OBJS)

# $(OBJS) :
#	$(CC) -c SRC -o $@

clean:
	rm -f $(OBJS_DIR)*.o *.exe

