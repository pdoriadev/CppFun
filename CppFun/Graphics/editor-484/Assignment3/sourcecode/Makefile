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
    EXE = editor.exe
else
    EXE = editor
endif

IMGUI_HEADERS = include/imgui
IMGUI_DIR = src/imgui
TINYDIALOG_HEADERS = include/tinydialog
TINYDIALOG_DIR = src/tinydialog
SRC_HEADER = include
SRC_DIR = src
OBJ_DIR = obj
GLAD_HEADER = include/glad
GLAD_DIR = src/glad
STB_DIR = src/stb

SOURCES = main.cpp
SOURCES += $(GLAD_DIR)/glad.c
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
SOURCES += $(TINYDIALOG_DIR)/tinyfiledialogs.c
SOURCES += $(SRC_DIR)/Shape.cpp $(SRC_DIR)/Teapot.cpp $(SRC_DIR)/Mobius.cpp $(SRC_DIR)/Torus.cpp $(SRC_DIR)/ImportShape.cpp $(SRC_DIR)/ImportCurve.cpp $(SRC_DIR)/ImportCharacter.cpp $(SRC_DIR)/Curve.cpp $(SRC_DIR)/Joint.cpp $(SRC_DIR)/MatrixStack.cpp $(SRC_DIR)/SkeletalModel.cpp $(SRC_DIR)/ColorPresets.cpp $(SRC_DIR)/FileImporter.cpp $(SRC_DIR)/Renderer.cpp $(SRC_DIR)/ShapeManager.cpp $(SRC_DIR)/TimeStepper.cpp $(SRC_DIR)/ParticleSystem.cpp $(SRC_DIR)/SimpleSystem.cpp $(SRC_DIR)/PendulumSystem.cpp  $(SRC_DIR)/SimplePendulum.cpp $(SRC_DIR)/SimpleChain.cpp $(SRC_DIR)/SimpleCloth.cpp $(SRC_DIR)/Application.cpp $(SRC_DIR)/Globals.cpp
SOURCES += $(SRC_DIR)/ErrorHandling.cpp $(SRC_DIR)/ShaderLoader.cpp
# Ray casting / ray tracing
SOURCES += $(SRC_DIR)/Camera.cpp $(SRC_DIR)/SceneLight.cpp $(SRC_DIR)/BVH.cpp $(SRC_DIR)/RayScene.cpp $(SRC_DIR)/RayTracer.cpp $(SRC_DIR)/ImageIO.cpp
SOURCES += $(SRC_DIR)/Settings.cpp $(SRC_DIR)/Light.cpp $(SRC_DIR)/SceneIO.cpp $(SRC_DIR)/AnimationController.cpp
SOURCES += $(SRC_DIR)/HeadPose.cpp $(SRC_DIR)/VRSystem.cpp
# Keyframe animation (AnimationTimeline is given; Keyframe.cpp is the assignment)
SOURCES += $(SRC_DIR)/AnimationTimeline.cpp
# Texture mapping. TextureCache (decode + GL upload) and TexCoordsGiven
# (planarPerFace, parametricGrid) are given; the UV generation and sampling in
# TexCoords.cpp are the assignment. TexCoordsGiven was split out because
# planarPerFace is the same dominant-axis dispatch the student's cube() needs --
# see the header comment in that file.
SOURCES += $(SRC_DIR)/TextureCache.cpp $(SRC_DIR)/TexCoordsGiven.cpp
# The given halves of curves and skinning stay in the list above
# (Curve.cpp, ImportCurve.cpp, SkeletalModel.cpp, Joint.cpp, MatrixStack.cpp,
# ImportCharacter.cpp). Their student halves -- CurveEval.cpp, Surface.cpp and
# Skinning.cpp -- are swappable units declared below, so they must NOT appear
# here or the solution switch would compile them anyway.

##---------------------------------------------------------------------
## PER-TOPIC SOLUTION LIBRARIES
##---------------------------------------------------------------------
##
##   make                          build the student's own code (the default)
##   make PARTICLES=solution       link the prebuilt particle-systems solution
##   make solution-lib-particles   instructor: build lib/solution/libparticles.a
##   make solution-libs            instructor: build every archive
##
## SWITCHES ARE KEYED BY TOPIC, NOT BY ASSIGNMENT NUMBER.
## CPSC 566 and CPSC 484 number the same material differently -- particle
## systems are 566's Assignment 5, the mesh loader is 484's Assignment 3 -- so
## a variable called "A3" means two different things depending on which course
## is holding the Makefile. Topic names do not collide.
##
## WHY THIS IS A BUILD SWITCH AND NOT JUST "ALSO LINK THE LIBRARY":
## a static archive member is only extracted if it resolves a symbol that is
## still UNDEFINED. If the student's .o is on the link line and already defines
## takeStep(), the archive member is never pulled -- the library is silently
## ignored, with no error and no warning. So the student source has to be
## excluded from the build, not merely accompanied by the library.
##
## Link order matters too: the archive must come AFTER the objects that need
## it. Listing it first makes the linker pull the member eagerly and then
## collide with the student object ("multiple definition").
##
## The swappable unit is a whole translation unit, which is why Integrators.cpp
## and PendulumSystemForces.cpp were split out of their parent files.
##
## TO ADD A TOPIC: define <topic>_SRC, then add one $(eval $(call ...)) line
## and the topic's name to SOLUTION_TOPICS. Nothing else changes.

SOLUTION_LIB_DIR = lib/solution

# ODE integration and spring forces.  566: A5 Particle Systems.
particles_SRC = $(SRC_DIR)/Integrators.cpp $(SRC_DIR)/PendulumSystemForces.cpp

# Model matrix composition.  484: A2 Modeling and Transformations.
transforms_SRC = $(SRC_DIR)/ShapeTransform.cpp

# Wavefront OBJ parsing.  484: A3 Mesh Loader.
objparser_SRC = $(SRC_DIR)/ObjParser.cpp

# Orbit, pan and zoom.  A3 Mesh Loading and Camera Control, both courses -- the
# second half of it, alongside `objparser`.
#
# Only the three functions that CHANGE the camera's state are swappable.
# updatePosition(), setView(), the view and projection matrices and
# generateRay() stay in Camera.cpp because they are given and because a stubbed
# updatePosition() would render every scene in every assignment black -- see the
# header comment in CameraControl.cpp.
camera_SRC = $(SRC_DIR)/CameraControl.cpp

# VR view composition.  566: A3 VR View.
vrcamera_SRC = $(SRC_DIR)/VRCamera.cpp

# Keyframe interpolation.  484: A4 Animation.
animation_SRC = $(SRC_DIR)/Keyframe.cpp

# UV generation and texture sampling.  484: A5 Textures and Materials.
textures_SRC = $(SRC_DIR)/TexCoords.cpp

# Bezier and B-spline evaluation, surfaces of revolution and generalized
# cylinders.  566: A3 Curves and Surfaces.
curves_SRC = $(SRC_DIR)/CurveEval.cpp $(SRC_DIR)/Surface.cpp

# Joint transforms and skin deformation.  566: A4 Hierarchical Modeling & SSD.
skinning_SRC = $(SRC_DIR)/Skinning.cpp

# Whitted shading.  484: A6 Ray Caster / Ray Tracer, 566: A6.
raytracer_SRC = $(SRC_DIR)/Shading.cpp

# Shape generation and face normals.  A2 Geometry and Transformations, both
# courses -- the second half of it, alongside `transforms`.
#
# Six files in one topic. Multiple files per topic already works (curves_SRC has
# two), so unlike the bvh split no surgery was needed here: each generator is
# already its own translation unit. The one exception was
# Shape::calculateNormals(), which lived in Shape.cpp among 260 lines of given
# plumbing and had to move to ShapeNormals.cpp to become swappable.
geometry_SRC = $(SRC_DIR)/Cube.cpp $(SRC_DIR)/Pyramid.cpp $(SRC_DIR)/Sphere.cpp \
               $(SRC_DIR)/Icosahedron.cpp $(SRC_DIR)/Custom.cpp \
               $(SRC_DIR)/ShapeNormals.cpp

# Phong local illumination: light direction, distance falloff, and the
# ambient/diffuse/specular terms.  484: A5 Textures and Materials (second half),
# 566: part of A6.
#
# Its own topic rather than part of `textures` or `raytracer` so it can be
# promoted to a standalone 484 assignment later without touching any code --
# only ASSIGNMENTS.md and the weight table change.
lighting_SRC = $(SRC_DIR)/Lighting.cpp

# Ray/triangle and ray/box intersection, the BVH build, and the three
# traversals.  484: A6 Ray Caster / Ray Tracer, 566: A6.
#
# The second half of A6, split from `raytracer` so the assignment can be marked
# and rescued in two pieces: a student whose shading is right but whose
# traversal is broken renders a black image, and vice versa, and the two are
# separately diagnosable only if they are separately swappable.
bvh_SRC = $(SRC_DIR)/RayIntersect.cpp

# Topics with an archive that can be built. Keep in step with the eval lines.
SOLUTION_TOPICS = transforms geometry objparser camera curves skinning animation textures lighting raytracer bvh particles vrcamera

# $(call SOLUTION_SWITCH,<topic>,<SWITCH_VARIABLE>)
# Either compile the student's sources or link the archive instead -- never
# both, for the reason in the comment above.
define SOLUTION_SWITCH
ifeq ($$($(2)),solution)
    SOLUTION_LIBS += $$(SOLUTION_LIB_DIR)/lib$(1).a
    STALE_OBJS += $$(patsubst $$(SRC_DIR)/%.cpp,$$(OBJ_DIR)/%.o,$$($(1)_SRC))
else
    SOURCES += $$($(1)_SRC)
endif
endef

$(eval $(call SOLUTION_SWITCH,transforms,TRANSFORMS))
$(eval $(call SOLUTION_SWITCH,geometry,GEOMETRY))
$(eval $(call SOLUTION_SWITCH,objparser,OBJPARSER))
$(eval $(call SOLUTION_SWITCH,camera,CAMERA))
$(eval $(call SOLUTION_SWITCH,curves,CURVES))
$(eval $(call SOLUTION_SWITCH,skinning,SKINNING))
$(eval $(call SOLUTION_SWITCH,animation,ANIMATION))
$(eval $(call SOLUTION_SWITCH,textures,TEXTURES))
$(eval $(call SOLUTION_SWITCH,lighting,LIGHTING))
$(eval $(call SOLUTION_SWITCH,raytracer,RAYTRACER))
$(eval $(call SOLUTION_SWITCH,bvh,BVH))
$(eval $(call SOLUTION_SWITCH,vrcamera,VRCAMERA))
$(eval $(call SOLUTION_SWITCH,particles,PARTICLES))

# PRUNE STALE OBJECTS AT PARSE TIME, not in a recipe.
#
# Switching a topic to `solution` drops its .cpp from SOURCES, but an .o from a
# previous build stays on disk -- and tests/Makefile.test links whatever
# `$(wildcard obj/*.o)` finds. That object defines the same symbols as the
# archive member, so the member is never extracted and the archive is silently
# ignored:
#
#     make TEXTURES=student  && ./texture_test  ->  25 failures   (correct)
#     make TEXTURES=solution && ./texture_test  ->  25 failures   (WRONG)
#
# A grader who forgets to clean between configurations marks the student's stub
# while believing they are marking the reference, and nothing says so.
#
# This has to run at PARSE time rather than in the link recipe: after a switch
# the editor is often already up to date, so the recipe never fires -- but
# `make -f tests/Makefile.test` still globs the stale object afterwards.
#
# NOTE this does not save you from a stale TEST BINARY, which `make clean` also
# leaves behind. Use `make distclean` when grading. See INSTRUCTOR-GUIDE.md.
ifneq ($(strip $(STALE_OBJS)),)
$(foreach o,$(wildcard $(STALE_OBJS)),$(info   [pruned stale $(o) -- that topic now comes from an archive]))
$(shell rm -f $(STALE_OBJS) $(STALE_OBJS:.o=.d))
endif

# Backwards compatibility: A3=solution was the old spelling of PARTICLES=solution.
ifeq ($(A3),solution)
$(error A3=solution has been renamed. Use PARTICLES=solution -- switches are \
keyed by topic now, because 566 and 484 number the assignments differently)
endif

# Every file that any topic can swap out. check-sources skips these, since they
# are legitimately absent from SOURCES whenever their archive is in use.
SWAPPABLE_SRC = $(foreach t,$(SOLUTION_TOPICS),$($(t)_SRC))

# Object files (in obj directory)
OBJS = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
UNAME_S := $(shell uname -s)

# A literal comma, for use inside $(if ...) where a bare one would split args.
, := ,

CXXFLAGS = -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(IMGUI_HEADERS) -I$(TINYDIALOG_HEADERS) -I$(SRC_HEADER) -I$(GLAD_HEADER) -I$(STB_DIR)
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

# SOLUTION ARCHIVES MUST BE LINKED AS A GROUP.
#
# A static archive is searched ONCE, in the order it appears, and only for
# symbols undefined at that moment. The archives are emitted in the order the
# SOLUTION_SWITCH lines above run -- lighting before raytracer -- but the
# dependency runs the other way: Shading.cpp (raytracer) calls lightGeometry()
# and phongContribution() (lighting). So
#
#     make LIGHTING=solution RAYTRACER=solution
#
# put liblighting.a first, the linker passed it before anything needed it, and
# the build died with
#
#     undefined reference to `lightGeometry(SceneLightSample const&, ...)'
#
# That combination is not exotic -- it is exactly what grading A6 needs from a
# student who never finished A5's lighting, and what grading A5's textures
# fully needs. Reordering the list would fix today's pair and break on the next
# dependency someone adds; --start-group makes the linker re-scan until nothing
# is left undefined, so the order stops mattering at all.
#
# ld64 (macOS) has no --start-group and does not need one: it already re-scans
# archives. Passing the flag there is an error, so branch.
ifeq ($(UNAME_S), Darwin)
    SOLUTION_LINK = $(SOLUTION_LIBS)
else
    SOLUTION_LINK = $(if $(SOLUTION_LIBS),-Wl$(,)--start-group $(SOLUTION_LIBS) -Wl$(,)--end-group,)
endif

$(EXE): $(OBJS) $(SOLUTION_LIBS)
	@if [ -n "$(SOLUTION_LIBS)" ]; then echo "  [solution libraries: $(SOLUTION_LIBS)]"; fi
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(SOLUTION_LINK) $(LIBS)

# Builds lib/solution/lib<topic>.a from whatever <topic>_SRC currently contains.
# Run this from the INSTRUCTOR repo, where those files hold the real
# implementations; the resulting archive is what ships to students.
#
# One rule serves every topic. The archive is rebuilt from source each time
# rather than reusing obj/, so it can never pick up an object compiled with a
# different set of flags than the archive is meant to carry.
solution-lib-%:
	@mkdir -p $(SOLUTION_LIB_DIR) $(OBJ_DIR)/solution
	@if [ -z "$($*_SRC)" ]; then \
	  echo "No sources defined for topic '$*'. Known topics: $(SOLUTION_TOPICS)"; \
	  exit 1; \
	fi
	@for f in $($*_SRC); do \
	  echo "  CXX $$f"; \
	  $(CXX) $(CXXFLAGS) -c $$f -o $(OBJ_DIR)/solution/`basename $$f .cpp`.o || exit 1; \
	done
	@rm -f $(SOLUTION_LIB_DIR)/lib$*.a
	ar rcs $(SOLUTION_LIB_DIR)/lib$*.a \
	    $(foreach f,$($*_SRC),$(OBJ_DIR)/solution/$(notdir $(basename $(f))).o)
	@echo "Built $(SOLUTION_LIB_DIR)/lib$*.a"

# Build every archive at once, for a release to students.
solution-libs:
	@for t in $(SOLUTION_TOPICS); do $(MAKE) solution-lib-$$t || exit 1; done

clean:
	rm -f $(EXE) $(OBJS) $(OBJS:.o=.d)
	rm -rf $(OBJ_DIR)

# Everything clean removes, plus the test binaries and the images they emit.
distclean: clean
	$(MAKE) -f tests/Makefile.test clean

# "Is the Makefile up to date?" - lists any src/*.cpp that SOURCES does not
# mention. Two files are excluded on purpose: CharacterShape.cpp and
# FileManager.cpp are dead code that does not compile (CharacterShape::draw()
# does not match Shape's signature; FileManager is commented out of
# Application.h), so they must stay out of the build.
check-sources:
	@missing=""; \
	for f in $(wildcard $(SRC_DIR)/*.cpp); do \
	  b=`basename $$f`; \
	  skip=no; \
	  case "$$b" in CharacterShape.cpp|FileManager.cpp) skip=yes;; esac; \
	  for s in $(notdir $(SWAPPABLE_SRC)); do [ "$$b" = "$$s" ] && skip=yes; done; \
	  [ "$$skip" = yes ] && continue; \
	  echo "$(SOURCES)" | grep -q "$$b" || missing="$$missing $$b"; \
	done; \
	if [ -n "$$missing" ]; then \
	  echo "Not in SOURCES:$$missing"; exit 1; \
	else \
	  echo "SOURCES is complete."; \
	fi

.PHONY: all clean distclean check-sources solution-libs

# Pull in the generated header dependencies (ignored on the first build, when
# none exist yet).
-include $(OBJS:.o=.d)
