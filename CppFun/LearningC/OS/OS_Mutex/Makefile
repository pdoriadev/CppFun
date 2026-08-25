CC = g++
CFLAGS = -g 

OBJS = mutexMain.o

mutexMain.exe : $(OBJS)
	@echo "------------------------"
	@echo "-- Linking Executable --"
	$(CC) $^ -o mutexMain.exe

%.o : %.cpp
	@echo "-----------------------"
	@echo "------ COMPILING ------"
	$(CC) -c $<
