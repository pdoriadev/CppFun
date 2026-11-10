#############################################################################
# FLAGS

# Catalogue of flags and built-in rules: https://www.gnu.org/software/make/manual/make.html#Catalogue-of-Rules
#Flag for .c file implicit rules
#CC = gcc 

# Flag for implicit rules. 
#CFLAGS = -g #Turn on debug info.

# https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html
# -M - user dependencies
#CPPFLAGS = -M

#############################################################################
# VARIABLES

# wildcard function at immediate time to create a list of all .c files in \
	# the source directory.
SRC_DIR := src
SRCS := $(wildcard $(SRC_DIR)/*.c)


# OBJ_DIR variable assigned at 'immediate' time. Also known as read time. i
# https://www.gnu.org/software/make/manual/make.html#Reading-Makefiles
OBJ_DIR := obj
# Create a list of .c files in current directory. \
	# For each list item ending in .c, replace '.c' with '.o'. \
	# Creates a list of .o files that correspond to each .c file. \
	# For each list item prepends the OBJ_DIRectory's prefix. \
	# https://www.gnu.org/software/make/manual/make.html#Prerequisite-Types

OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(SRCS))
OBJS := $(subst .c,.o, $(OBJS))

#OBJS := $(subst .c, .o, $(subst $(SRC_DIR), $(OBJ_DIR), $(SRCS)))
#OBJS := $(patsubt $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, src/file1.c) 
#OBJS := $(patsubt %.c, %.o, $(wildcard $(SRC_DIR)/*.c)) 
#addprefix $(OBJ_DIR)/, $(
EXE := buildexecutable
EXE_DIR := build

#############################################################################
# RULES

# Rule for building *.o files
# Automatic variables reference (i.e. $<, $@): https://www.gnu.org/software/make/manual/make.html#Automatic-Variables
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@#@echo "Stem-rule for building .o files from .c files"	
	gcc --compile $< --output $@
	@#$(CC) $(CFLAGS) $(CPPFLAGS) -c $<
	@#$(COMPILE.c) $(OUTPUT_OPTION) $+
	@#$(CC) -o $(EXE_DIR)/$(BUILD).exe $(OBJ_DIR)/*.o

# Implicit rule #1: object files are built via the C linker implicit rule
# Implicit rule #2: blah.o is built via the C compilation implicit rule, because blah.c exists
all: DIRECTORIES_BUILT $(OBJS) 
	gcc $(OBJS) --output $(EXE)
	@#@echo $(SRC_DIR)
	@#@echo $(SRCS)
	@#@echo $(OBJS)
	@#@echo $@
	@#@echo "all rule"

# order-only prerequisite with pipe operator. \
	# Creates an obj directory if it doesn't exist. \
	# \
	# Directory's 'last modified' timestamp is updated every time one of \
	# its files is modified. That means the $(OBJ_DIR)'s matching target \ won't be triggered if $(OBJ_DIR) already exists. \
	# https://www.gnu.org/software/make/manual/make.html#Prerequisite-Types
DIRECTORIES_BUILT : | $(OBJ_DIR) $(EXE_DIR)
	@echo "DIRECTORIES_BUILT RULE"

$(OBJ_DIR) :
	@echo "Create OBJ Directory"
	mkdir $(OBJ_DIR)/

$(EXE_DIR) :
	@echo "Create Build Directory"
	mkdir $(EXE_DIR)/

##############################################################################
# PHONY targets

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(EXE)

# Reads make file. Assigns immediate-time variables.
# First target is 'all'. Becomes goal target. 
# Looks at goal target's pre-reqs. For each *.o file in $(OBJS), runs an \
	# implicit rule, checks that the *.o file exists and if it is newer than \
	# its matching sourcefile. If the *.o file doesn't exist, or it \
	# is older than its matching sourcefile, make will look for a rule \
	# to build the .o file. It should find one. \
