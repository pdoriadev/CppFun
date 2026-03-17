# Use gcc for linking and compilation instead of cc
CC = gcc
# Use debug flag
CFLAGS = -g

# Implicit rule #1: object files are built via the C linker implicit rule
# Implicit rule #2: *.o files built via the C compilation implicit rule, because>

SOURCE_DIR = /SOURCE/
OBJS_DIR = /OBJS/

SRCS := AccountActions.c BankAccount.c ErrorLog.c ThreadLog.c main.c 
OBJS := AccountActions.o BankAccount.o ErrorLog.o ThreadLog.o main.o  

all: $(SRCS) $(OBJS)
	@echo "---------------------------------"
	@echo "------- LINKING EXECUTABLE ------"
	$(CC) $(SRCS) -o ./build.exe

%.o : %.c
	@echo "---------------------------------"
	@echo "------- COMPILING C FILES -------"
	$(CC) -c $<

clean:
	rm -f $(OBJS_DIR)*.o *.exe

