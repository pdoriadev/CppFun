# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Libraries and includes
LIBS = -lglfw -ldl -lGL
INCLUDES = -I.

# Source and object files
SRC = main.cpp glad.c
OBJ = main.o glad.o

# Output executable name
TARGET = assignment_0

# Default make target
all: $(TARGET)

# Compile the program
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LIBS)

# Compile C++ source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile C source files (for glad.c)
%.o: %.c
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean compiled files, but DO NOT delete glad.c!
clean:
	rm -f $(OBJ) $(TARGET)

