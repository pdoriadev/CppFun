# Adapted from Dave Garcia-Gomez's CPSC 484 class' Makefile.
#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# Linux:
#   apt-get install libglfw3-dev
#

#CXX = g++
#CXX = clang++

CC = g++
CFLAGS = -Iinclude -std=c++11

# MinGW's g++ appends .exe when the -o name has no extension, so the file on
# disk is editor.exe while EXE said "editor" - and `make clean` then quietly
# failed to delete it.
ifeq ($(OS), Windows_NT)
    EXE = scratch.exe
else
    EXE = scratch
endif

# ifeq ([-e ./$(EXE)], 1)
# 	rm $(EXE)
# 	echo "Removed old executable."
# endif

# $(EXE) :
# 	touch $(EXE)

SRC_HEADER = include
SRC_DIR = src
OBJ_DIR = obj
GLAD_HEADER = include/glad
GLAD_DIR = src/glad

SOURCES = main.cpp
SOURCES += $(GLAD_DIR)/glad.c
SOURCES += $(SRC_DIR)/main.cpp $(SRC_DIR)/InputCache.cpp $(SRC_DIR)/Logging.cpp $(SRC_DIR)/Modling.cpp $(SRC_DIR)/shaderSource.cpp $(SRC_DIR)/utils.cpp
# SOURCES += $(SRC_DIR)/ErrorHandling.cpp $(SRC_DIR)/ShaderLoader.cpp <-- Files like this look really useful
# Ray casting / ray tracing
# SOURCES += $(SRC_DIR)/Camera.cpp $(SRC_DIR)/SceneLight.cpp $(SRC_DIR)/BVH.cpp $(SRC_DIR)/RayScene.cpp $(SRC_DIR)/RayTracer.cpp $(SRC_DIR)/ImageIO.cpp
# SOURCES += $(SRC_DIR)/Settings.cpp $(SRC_DIR)/Light.cpp $(SRC_DIR)/SceneIO.cpp $(SRC_DIR)/AnimationController.cpp
# Keyframe animation (AnimationTimeline is given; Keyframe.cpp is the assignment)
# SOURCES += $(SRC_DIR)/AnimationTimeline.cpp
# Texture mapping. TextureCache (decode + GL upload) and TexCoordsGiven
# (planarPerFace, parametricGrid) are given; the UV generation and sampling in
# TexCoords.cpp are the assignment. TexCoordsGiven was split out because
# planarPerFace is the same dominant-axis dispatch the student's cube() needs --
# see the header comment in that file.
# SOURCES += $(SRC_DIR)/TextureCache.cpp $(SRC_DIR)/TexCoordsGiven.cpp
# The given halves of curves and skinning stay in the list above
# (Curve.cpp, ImportCurve.cpp, SkeletalModel.cpp, Joint.cpp, MatrixStack.cpp,
# ImportCharacter.cpp). Their student halves -- CurveEval.cpp, Surface.cpp and
# Skinning.cpp -- are swappable units declared below, so they must NOT appear
# here or the solution switch would compile them anyway.

# Object files (in obj directory)
OBJS = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
UNAME_S := $(shell uname -s)

CXXFLAGS = -std=c++11 -I$(SRC_HEADER) -I$(GLAD_HEADER)
CXXFLAGS += -g -Wall -Wformat
CXXFLAGS += -DGLM_ENABLE_EXPERIMENTAL
# Emit a .d file per object listing the headers it includes. Without this,
# editing a header recompiles nothing that includes it, and you silently link
# objects built against different struct layouts.
CXXFLAGS += -MMD -MP


##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += -lGL -lglfw -ldl
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"

	# HOMEBREW'S PREFIX DEPENDS ON THE ARCHITECTURE, and this is the single
	# most common way a Mac build fails:
	#
	#     Intel          /usr/local      - already on the compiler's default
	#                                      search path, so a build with no -I
	#                                      at all happens to work
	#     Apple Silicon  /opt/homebrew   - NOT on the default search path
	#
	# So hardcoding /usr/local works on an Intel Mac and fails on every
	# M-series one with:
	#
	#     fatal error: 'GLFW/glfw3.h' file not found
	#
	# Ask brew where it lives rather than guessing, and fall back to the
	# Apple Silicon location if brew is not on PATH. /opt/local is MacPorts.
	BREW_PREFIX := $(shell brew --prefix 2>/dev/null)
	ifeq ($(BREW_PREFIX),)
		BREW_PREFIX := /opt/homebrew
	endif

	# -lglfw was missing here. The frameworks satisfy the OS-level symbols
	# but nothing was supplying GLFW itself, so the compile could succeed and
	# the link still fail with undefined _glfwInit and friends.
	LIBS += -lglfw
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LIBS += -L$(BREW_PREFIX)/lib -L/usr/local/lib -L/opt/local/lib
	CXXFLAGS += -I$(BREW_PREFIX)/include -I/usr/local/include -I/opt/local/include
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "MinGW"
	LIBS += -lgdi32 -lopengl32 -lglfw3
	# tinyfiledialogs calls straight into the Win32 API and needs these:
	#   comdlg32 - GetOpenFileName / GetSaveFileName / ChooseColor
	#   ole32    - CoInitializeEx / CoUninitialize
	#   shell32  - SHBrowseForFolder / SHGetPathFromIDList (folder picker)
	#   uuid     - the IID_/CLSID_ constants those COM calls reference
	LIBS += -lcomdlg32 -lole32 -lshell32 -luuid
	CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

$(OBJ_DIR)/%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o:$(TINYDIALOG_DIR)/%.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o:$(GLAD_DIR)/%.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

# obj/ is gitignored, so a fresh clone has no such directory and the
# first compile would fail with "No such file or directory". Create it
# as an order-only prerequisite: needed before the objects, but its
# timestamp must not force rebuilds.
$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(EXE): $(OBJS) $(SOLUTION_LIBS)
	@if [ -n "$(SOLUTION_LIBS)" ]; then echo "  [solution libraries: $(SOLUTION_LIBS)]"; fi
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(SOLUTION_LINK) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS) $(OBJS:.o=.d)
	rm -rf $(OBJ_DIR)

# "Is the Makefile up to date?" - lists any src/*.cpp that SOURCES does not
# mention. Two files are excluded on purpose: CharacterShape.cpp and
# FileManager.cpp are dead code that does not compile (CharacterShape::draw()
# does not match Shape's signature; FileManager is commented out of
# Application.h), so they must stay out of the build.
# check-sources:
# 	@missing=""; \
# 	for f in $(wildcard $(SRC_DIR)/*.cpp); do \
# 	  b=`basename $$f`; \
# 	  skip=no; \
# 	  case "$$b" in CharacterShape.cpp|FileManager.cpp) skip=yes;; esac; \
# 	  for s in $(notdir $(SWAPPABLE_SRC)); do [ "$$b" = "$$s" ] && skip=yes; done; \
# 	  [ "$$skip" = yes ] && continue; \
# 	  echo "$(SOURCES)" | grep -q "$$b" || missing="$$missing $$b"; \
# 	done; \
# 	if [ -n "$$missing" ]; then \
# 	  echo "Not in SOURCES:$$missing"; exit 1; \
# 	else \
# 	  echo "SOURCES is complete."; \
# 	fi
# 

# ME ^^^ I don't know how to bash/shell this righ tnow. Can come back to later. 
#		The functionality looks very useful.  

.PHONY: all clean #check-sources


# Pull in the generated header dependencies (ignored on the first build, when
# none exist yet).
-include $(OBJS:.o=.d)
