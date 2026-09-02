// =============================================================================
// CPSC 484 - Assignment 0 - Demo / Environment Setup
// =============================================================================
// LINE-BY-LINE COMMENTED REFERENCE VERSION
//
//
// This is a demo, not a graded deliverable. Your job for Assignment 0 is to
// get your environment set up, get this compiling and running, and then
// read through it until you understand every piece.
//
// WHAT THIS PROGRAM DOES
//   It spells out a string "CPSC 484" by default, using solid,
//   extruded 3D "block letters." Each letter is built from a handful of
//   rectangular strokes (e.g. the letter H is three bars: two vertical
//   sides and a horizontal crossbar); each stroke is pushed out into a
//   small 3D box instead of staying a flat 2D shape. The letters spin
//   around their vertical (Y) axis, cycle through colors over time, and
//   are shaded with a simple fixed light so their 3D shape is easy to see.
//
// HOW THE PIECES FIT TOGETHER (read this before diving into the code)
//   1. We ask GLFW to open a window and give us an OpenGL context.
//   2. We ask GLAD to load the actual OpenGL function pointers for us
//      (GLFW doesn't know how to call OpenGL itself -- GLAD is what makes
//      functions like glDrawArrays actually available to call).
//   3. We compile two small GPU programs (a vertex shader + a fragment
//      shader) into a "shader program" that will run on every vertex and
//      every pixel we draw.
//   4. For every letter that appears in our string, we build a list of 3D
//      triangles (position + normal per vertex) on the CPU, upload it to
//      GPU memory ONCE (a VBO), and record how to interpret that memory
//      (a VAO) -- this happens at startup, not every frame.
//   5. Every frame, for every letter, we compute a transformation matrix
//      (rotate + scale + position it along the string) and a color, send
//      both to the shader as "uniforms," and issue one draw call.
//   6. Keyboard input is read two different ways in this file on purpose
//      -- see the "INPUT HANDLING" section below for why.
//
// =============================================================================
/*

m


*/


#include "glad.h"          // OpenGL function loader -- must be included before glfw3.h
#include <GLFW/glfw3.h>    // window/context creation, input, timing
#include <cctype>          // std::toupper, std::tolower
#include <cmath>           // sin, cos, M_PI
#include <cstdlib>         // std::getenv -- used by isRunningUnderWSL() below
#include <fstream>         // std::ifstream -- used by isRunningUnderWSL() below
#include <iostream>        // std::cerr for error messages
#include <unordered_map>   // per-letter geometry and GPU buffer lookup tables
#include <string>          // displayString / titleString
#include <vector>          // dynamically-sized vertex lists

// -----------------------------------------------------------------------------
// PROGRAM STATE -- the string being displayed, and the window's title.
// Try changing displayString and re-running: any letter A-Z, space, or '-'
// works, because that's what kLetterRects (further down) defines shapes for.
// -----------------------------------------------------------------------------

std::string displayString = "Peter"; 						// the text that gets spelled out in 3D letters
std::string titleString = "Assignment 0 - Peter";       // the text shown in the OS window's title bar

/* *************************************************** */

// -----------------------------------------------------------------------------
// FUNCTION PROTOTYPES
// C++ requires a function to be declared before it's used. main() (further
// down) calls all of these, so we declare their signatures up here and give
// the actual bodies later in the file -- keeps main() readable from the top.
// -----------------------------------------------------------------------------

void framebuffer_size_callback(GLFWwindow* window, int width, int height);              // called by GLFW whenever the window is resized
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);     // called by GLFW on every key press/release/repeat
void processInput(GLFWwindow* window);                                                  // polled once per frame for continuously-held keys
unsigned int compileShader(unsigned int type, const char* source);                      // compiles one GLSL shader, returns its OpenGL ID
unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc);        // compiles + links both shaders into one usable program

// -----------------------------------------------------------------------------
// SHADER SOURCES
// -----------------------------------------------------------------------------
// These are tiny GLSL programs that run ON THE GPU, not the CPU. We write
// them here as C++ raw strings (the R"( ... )" syntax lets the text span
// multiple lines without needing \n and quote escapes) and hand them to
// OpenGL to compile at runtime, the same way g++ compiles this .cpp file.
//
// This vertex/fragment pair includes a small, FIXED amount of shading
// (ambient + diffuse against a light that never moves) purely so the
// extruded letters read as solid 3D shapes instead of flat-colored
// silhouettes. Don't worry about the lighting math yet -- we cover diffuse
// shading properly in Module 3. The one thing worth noticing now: the
// normal vector gets multiplied by the same `transform` matrix as the
// position (via its upper-left 3x3, mat3(transform)), because when an
// object rotates, the directions its surfaces face rotate with it too.
// That only works this simply because every letter here is rotated and
// *uniformly* scaled -- non-uniform scaling would tilt the normals and
// need a correction (the "normal matrix") you'll meet later in the course.

// NOTE ON R"GLSL(...)GLSL" below: a plain raw string R"( ... )" ends at the
// FIRST occurrence of the two characters )" anywhere inside it -- it has no
// idea it's sitting inside a GLSL comment. This file's shader comments
// contain both parentheses and quotes (e.g. a comment mentioning
// "layout (location = N)"), and )" showed up by accident in the middle of
// one, silently truncating the string and turning the rest of the shader
// text into (invalid) C++ code. Using a named delimiter -- R"GLSL( ... )GLSL"
// -- changes the terminator to the much less accident-prone )GLSL", so this
// can't happen again no matter what the shader comments say.

const char* vertexShaderSource = R"GLSL(
#version 330 core
// "layout (location = N)" must match the glVertexAttribPointer(N, ...) calls
// in createLetterBuffer() below -- that's how the CPU-side vertex data gets
// matched up to these shader inputs.
layout (location = 0) in vec3 aPos;     // this vertex's position, straight from our VBO
layout (location = 1) in vec3 aNormal;  // this vertex's normal (which way its face points)

// "out" variables are computed once per vertex here, then automatically
// interpolated across each triangle before the fragment shader below sees
// them (that interpolation step is called rasterization).

out vec3 Normal;                        // will be picked up by "in vec3 Normal" in the fragment shader

// A "uniform" is a value we set once per draw call from the CPU (see
// glUniformMatrix4fv in the render loop) that stays constant across every
// vertex/pixel of that draw call -- unlike aPos/aNormal, which are
// different for every vertex.

uniform mat4 transform;                 // this letter's combined rotate+scale+position matrix, set from the CPU

void main() {                           // GLSL entry point -- runs once per vertex
    // mat3(transform) keeps only the rotation+scale part of the 4x4 matrix
    // (it drops the translation column), which is what you want when
    // transforming a *direction* like a normal instead of a *point*.
    Normal = mat3(transform) * aNormal; // rotate/scale this vertex's normal the same way the shape itself is rotated/scaled

    // gl_Position is a special built-in output: OpenGL reads it to know
    // where this vertex lands on screen (in clip space).
    gl_Position = transform * vec4(aPos, 1.0); // transform this vertex's position into its final on-screen location
}

)GLSL";

// PETER - WE ARE PASSING A WHOLE ASS `main` method to GLSL???

/*  PETER
vertex shader
HERE
- for every vertex. OR for every triangle?? 
    - I think this only makes sense for triangles, the way that Normals are used in main. 
- transform - is this the transform shared by all the vertices of a single letter?

frag shader
- for every pixel
0
main()
{
    HERE
    
    Normal = mat3(transform) * aNormal. 
    // Rotating/scaling the normal is same as rotating/scaling the face???

    HERE
    gl_Position = transform * vec4(aPos,1.0)
    // vec4() - Vector of 4 Floats. Column vector of size 4? 
        - https://thebookofshaders.com/glossary/?search=vec4
        - I think we're passing the aPos vector and scaling each value of aPos by 1. 
        
    
}


*/


const char* fragmentShaderSource = R"GLSL(
#version 330 core
out vec4 FragColor;      // the final pixel color -- this is the only required output
in vec3 Normal;          // interpolated from the vertex shader's "out vec3 Normal" above
uniform vec3 color;      // this letter's current color, set from the CPU each frame

void main() {                                           // GLSL entry point -- runs once per pixel (fragment)
    vec3 N = normalize(Normal);                         // interpolation can shrink the length; renormalize to unit length
    vec3 lightDir = normalize(vec3(0.4, 0.6, 1.0));      // a fixed light direction, never moves

    float ambient = 0.5;                                // a little light even on faces facing away from the light
    float diffuse = max(dot(N, lightDir), 0.0) * 1.2;    // brighter when a face points toward the light; clamp negative to 0

    FragColor = vec4(color * (ambient + diffuse), 1.0);  // scale this letter's color by the light amount; alpha = fully opaque
}
)GLSL";

// Screen dimensions -- passed to glfwCreateWindow() below.
const unsigned int SCR_WIDTH = 800;  // window width in pixels
const unsigned int SCR_HEIGHT = 600; // window height in pixels

// -----------------------------------------------------------------------------
// PER-LETTER ANIMATION STATE
// One entry per character in displayString: rotations[i] is how far letter i
// has spun so far (in degrees), colorOffsets[i] staggers letter i's color
// cycle so they don't all flash the same color at once. Both are resized to
// match displayString's length inside main(), once we know how long it is.
// -----------------------------------------------------------------------------

std::vector<float> rotations;     // rotations[i] = how far letter i has spun so far, in degrees
std::vector<float> colorOffsets;  // colorOffsets[i] = phase offset for letter i's color-cycling sine waves

// -----------------------------------------------------------------------------
// INPUT STATE
// See the INPUT HANDLING section (just above key_callback's definition) for
// why these are read in two different ways.
// -----------------------------------------------------------------------------

bool paused = false; // true while spinning is paused (toggled by SPACE); letters stop advancing when true

// Speed is a fixed set of LEVELS you step through one at a time, rather than
// a value that climbs for as long as a key is held. (An earlier version of
// this demo did the latter -- rotationSpeed crept up by a small amount every
// single FRAME while UP was held, which at ~60 frames/second meant a
// half-second tap already felt like "instantly full speed." Stepping
// through a short, fixed list on each individual key PRESS -- not each
// frame -- is the fix, and it's a better fit for "a handful of speeds to
// choose from" than continuous polling would be anyway.)

const std::vector<float> kSpeedLevels = {0.02f, 0.05f, 0.1f, 0.2f, 0.35f, 0.5f, 0.75f, 1.0f}; // the fixed speeds UP/DOWN step through
int speedLevelIndex = 2;                              // which kSpeedLevels entry is active now (index 2 = 0.1, the launch speed)
float rotationSpeed = kSpeedLevels[speedLevelIndex];  // degrees added to each letter's rotation per frame, when not paused

/* *************************************************** */
// =============================================================================
// 3D LETTER GEOMETRY
// =============================================================================
// Each letter is built from a handful of axis-aligned rectangular "strokes"
// in the z=0 plane. For example the letter 'T' is two strokes: a wide bar
// across the top, and a narrow bar down the middle. buildLetterVertices()
// (further down) extrudes every stroke into a solid box so the whole letter
// has real depth instead of being a flat cutout.

// A single rectangular stroke, described by its corners in the XY plane.

struct Rect {
    float xmin, ymin, xmax, ymax; // lower-left corner (xmin,ymin) and upper-right corner (xmax,ymax) of this stroke
};

// One or more rectangular strokes per letter. Coordinates roughly span
// [-0.6, 0.6] in both x and y. This table only defines shapes for uppercase
// A-Z, '-', and ' ' (space, which is empty on purpose -- it draws nothing).
// Each entry below has a trailing comment giving a rough plain-English
// description of what its strokes represent, to make the raw numbers easier
// to connect back to the letter's actual shape.

static const std::unordered_map<char, std::vector<Rect>> kLetterRects = {
    {'A', { {0.30f, -0.60f, 0.60f, 0.60f}, {-0.60f, -0.60f, -0.30f, 0.60f}, {-0.30f, 0.40f, 0.30f, 0.60f}, {-0.30f, -0.10f, 0.30f, 0.10f} }}, // 'A': right leg, left leg, top connecting bar, middle crossbar
    {'B', { {-0.60f, 0.40f, 0.40f, 0.60f}, {-0.60f, -0.60f, 0.40f, -0.40f}, {-0.60f, -0.10f, 0.40f, 0.10f}, {-0.60f, 0.10f, -0.30f, 0.40f}, {-0.60f, -0.40f, -0.30f, -0.10f}, {0.30f, 0.10f, 0.60f, 0.40f}, {0.30f, -0.40f, 0.60f, -0.10f} }}, // 'B': top cap, bottom cap, middle bar, left spine (upper+lower), right edges of the two bowls
    {'C', { {-0.60f, 0.40f, 0.60f, 0.60f}, {-0.60f, -0.60f, 0.60f, -0.40f}, {-0.60f, -0.40f, -0.30f, 0.40f} }}, // 'C': top cap, bottom cap, left spine (open on the right)
    {'D', { {-0.60f, 0.40f, 0.40f, 0.60f}, {-0.60f, -0.60f, 0.40f, -0.40f}, {-0.60f, -0.40f, -0.30f, 0.40f}, {0.30f, -0.40f, 0.60f, 0.40f} }}, // 'D': top cap, bottom cap, left spine, right edge (approximating the curve)
    {'E', { {-0.60f, 0.40f, 0.60f, 0.60f}, {-0.60f, -0.60f, 0.60f, -0.40f}, {-0.60f, -0.10f, 0.60f, 0.10f}, {-0.60f, 0.10f, -0.30f, 0.40f}, {-0.60f, -0.40f, -0.30f, -0.10f} }}, // 'E': top cap, bottom cap, middle bar, left spine (upper+lower)
    {'F', { {-0.60f, 0.40f, 0.60f, 0.60f}, {-0.60f, -0.10f, 0.60f, 0.10f}, {-0.60f, 0.10f, -0.30f, 0.40f}, {-0.60f, -0.60f, -0.30f, -0.10f} }}, // 'F': top cap, middle bar, left spine (upper+lower) -- no bottom cap
    {'G', { {-0.60f, 0.40f, 0.60f, 0.60f}, {-0.60f, -0.60f, 0.60f, -0.40f}, {-0.60f, -0.40f, -0.30f, 0.40f}, {0.30f, -0.40f, 0.60f, 0.10f} }}, // 'G': top cap, bottom cap, left spine, short right-side hook that distinguishes it from C
    {'H', { {-0.60f, -0.60f, -0.30f, 0.60f}, {0.30f, -0.60f, 0.60f, 0.60f}, {-0.30f, -0.10f, 0.30f, 0.10f} }}, // 'H': left leg, right leg, middle crossbar
    {'I', { {-0.15f, -0.60f, 0.15f, 0.60f} }}, // 'I': a single vertical bar
    {'J', { {0.30f, -0.60f, 0.60f, 0.60f}, {-0.60f, -0.60f, 0.40f, -0.40f}, {-0.60f, -0.40f, -0.30f, 0.10f} }}, // 'J': right leg (full height), bottom bar, left hook curling up
    {'K', { {-0.60f, -0.10f, 0.40f, 0.10f}, {-0.60f, 0.10f, -0.30f, 0.60f}, {-0.60f, -0.60f, -0.30f, -0.10f}, {0.30f, 0.10f, 0.60f, 0.60f}, {0.30f, -0.60f, 0.60f, -0.10f} }}, // 'K': left spine (upper+lower), middle notch, right-side diagonal strokes (upper+lower)
    {'L', { {-0.60f, -0.60f, -0.30f, 0.60f}, {-0.60f, -0.60f, 0.40f, -0.40f} }}, // 'L': left spine (full height) + bottom bar
    {'M', { {0.30f, -0.60f, 0.60f, 0.60f}, {-0.60f, -0.60f, -0.30f, 0.60f}, {-0.30f, 0.40f, 0.30f, 0.60f}, {-0.10f, -0.60f, 0.10f, 0.40f} }}, // 'M': left leg, right leg, top connecting bar, middle stroke (the dip)
    {'N', { {0.30f, -0.60f, 0.60f, 0.60f}, {-0.60f, -0.60f, -0.30f, 0.60f}, {-0.30f, 0.40f, 0.30f, 0.60f} }}, // 'N': left leg, right leg, diagonal-ish connecting bar
    {'O', { {0.30f, -0.60f, 0.60f, 0.60f}, {-0.60f, -0.60f, -0.30f, 0.60f}, {-0.30f, 0.40f, 0.30f, 0.60f}, {-0.30f, -0.60f, 0.30f, -0.40f} }}, // 'O': left side, right side, top bar, bottom bar (a rectangular ring)
    {'P', { {-0.60f, 0.40f, 0.60f, 0.60f}, {-0.60f, -0.10f, 0.60f, 0.10f}, {-0.60f, 0.10f, -0.30f, 0.40f}, {-0.60f, -0.60f, -0.30f, -0.10f}, {0.30f, 0.10f, 0.60f, 0.40f} }}, // 'P': top cap, middle bar, left spine (upper+lower), right edge of the upper bowl
    {'Q', { {0.30f, -0.40f, 0.60f, 0.60f}, {-0.60f, -0.40f, -0.30f, 0.60f}, {-0.30f, 0.40f, 0.30f, 0.60f}, {-0.30f, -0.40f, 0.30f, -0.20f}, {0.10f, -0.60f, 0.30f, -0.40f} }}, // 'Q': same ring shape as O, plus a small diagonal tail at the bottom-right
    {'R', { {-0.60f, -0.10f, 0.40f, 0.10f}, {-0.60f, 0.40f, 0.40f, 0.60f}, {-0.60f, 0.10f, -0.30f, 0.60f}, {-0.60f, -0.60f, -0.30f, -0.10f}, {0.30f, 0.10f, 0.60f, 0.40f}, {0.30f, -0.60f, 0.60f, -0.10f} }}, // 'R': like P (top cap, middle bar, left spine, right bowl edge) plus a diagonal leg at the bottom-right
    {'S', { {-0.60f, 0.40f, 0.60f, 0.60f}, {-0.60f, -0.60f, 0.60f, -0.40f}, {-0.60f, -0.10f, 0.60f, 0.10f}, {-0.60f, 0.10f, -0.30f, 0.40f}, {0.30f, -0.40f, 0.60f, -0.10f} }}, // 'S': top cap, bottom cap, middle bar, upper-left connector, lower-right connector
    {'T', { {-0.60f, 0.40f, 0.60f, 0.60f}, {-0.15f, -0.60f, 0.15f, 0.40f} }}, // 'T': top bar + vertical stem
    {'U', { {0.30f, -0.60f, 0.60f, 0.60f}, {-0.60f, -0.60f, -0.30f, 0.60f}, {-0.30f, -0.60f, 0.30f, -0.40f} }}, // 'U': left leg, right leg, bottom bar
    {'V', { {0.30f, 0.10f, 0.60f, 0.60f}, {-0.60f, 0.10f, -0.30f, 0.60f}, {0.10f, -0.40f, 0.40f, 0.10f}, {-0.40f, -0.40f, -0.10f, 0.10f}, {-0.20f, -0.60f, 0.20f, -0.40f} }}, // 'V': two upper diagonal strokes converging into a small bottom point (approximated with rectangles)
    {'W', { {0.30f, -0.60f, 0.60f, 0.60f}, {-0.60f, -0.60f, -0.30f, 0.60f}, {-0.30f, -0.60f, 0.30f, -0.40f}, {-0.10f, -0.40f, 0.10f, 0.40f} }}, // 'W': left leg, right leg, bottom bar, middle stroke (the center peak)
    {'X', { {0.30f, 0.10f, 0.60f, 0.60f}, {-0.60f, 0.10f, -0.30f, 0.60f}, {0.30f, -0.60f, 0.60f, -0.10f}, {-0.60f, -0.60f, -0.30f, -0.10f}, {-0.30f, -0.10f, 0.30f, 0.10f} }}, // 'X': four corner strokes plus a small middle piece where they cross
    {'Y', { {0.30f, 0.10f, 0.60f, 0.60f}, {-0.60f, 0.10f, -0.30f, 0.60f}, {-0.60f, -0.10f, 0.60f, 0.10f}, {-0.15f, -0.60f, 0.15f, -0.10f} }}, // 'Y': two upper diagonal strokes, a bar where they meet, a vertical stem below
    {'Z', { {-0.60f, 0.40f, 0.60f, 0.60f}, {-0.60f, -0.60f, 0.60f, -0.40f}, {-0.60f, -0.10f, 0.60f, 0.10f}, {0.30f, 0.10f, 0.60f, 0.40f}, {-0.60f, -0.40f, -0.30f, -0.10f} }}, // 'Z': top bar, bottom bar, middle diagonal-ish bar, corner strokes filling in the zigzag
    {'-', { {-0.60f, -0.10f, 0.60f, 0.10f} }}, // '-': a single thin horizontal bar
    {' ', {}}, // ' ' (space): no strokes at all -- draws nothing
};

// How far each stroke is extruded along +/-z, total letter thickness = 2 * kHalfDepth.
// Try changing this and re-running to see thicker/thinner letters.

static const float kHalfDepth = 0.10f; // half the letter's thickness along Z; total thickness = 2 * this

// Appends the 12 triangles that turn one rectangular stroke into a solid
// box: a front cap, a back cap, and four side walls connecting them.
//
//        top wall
//       +--------+  <- back face (z = zb, farther from viewer)
//      /        /|
//     +--------+ |
//     |        | +   <- right wall
//     | front  |/
//     +--------+     <- front face (z = zf, nearer to viewer)
//     bottom wall
//
// Each vertex carries a position AND a normal (6 floats/vertex, interleaved
// as x,y,z,nx,ny,nz). Since every face of a box is completely flat, each
// face just gets its own constant normal -- no averaging/interpolation
// needed the way you'd need for a curved surface like a sphere.

void appendExtrudedRect(std::vector<float>& out, const Rect& r, float halfDepth) {
    const float x0 = r.xmin, x1 = r.xmax, y0 = r.ymin, y1 = r.ymax; // pull the rectangle's four bounds into short local names
    const float zf = halfDepth;   // front face sits at +halfDepth along Z (nearer the viewer)
    const float zb = -halfDepth;  // back face sits at -halfDepth along Z (farther from viewer)

    // A little local helper (a C++ lambda) so we don't repeat this 9-position-
    // plus-3-normal pattern by hand twelve times below. [&] means it can see
    // and modify variables from the surrounding function, like `out`.
	
    auto triN = [&](float ax, float ay, float az,           // vertex A's position
                    float bx, float by, float bz,           // vertex B's position
                    float cx, float cy, float cz,           // vertex C's position
                    float nx, float ny, float nz) {          // the one normal shared by all 3 vertices (this face is flat)
        float v[18] = {
            ax, ay, az, nx, ny, nz,   // vertex A: position, then normal
            bx, by, bz, nx, ny, nz,   // vertex B: position, then normal
            cx, cy, cz, nx, ny, nz    // vertex C: position, then normal
        };
        out.insert(out.end(), v, v + 18); // append these 18 floats (3 full vertices) onto the end of the growing vertex list
    };

    // Front face (+z), split into 2 triangles, CCW when viewed from +z.
    // Every vertex on this face shares the same outward normal: (0,0,1).
    triN(x0, y0, zf,  x1, y0, zf,  x1, y1, zf,   0.0f, 0.0f, 1.0f); // front face, triangle 1 (lower-left, lower-right, upper-right)
    triN(x0, y0, zf,  x1, y1, zf,  x0, y1, zf,   0.0f, 0.0f, 1.0f); // front face, triangle 2 (lower-left, upper-right, upper-left)

    // Back face (-z), CCW when viewed from -z. Normal points the other way: (0,0,-1).
    triN(x0, y0, zb,  x1, y1, zb,  x1, y0, zb,   0.0f, 0.0f, -1.0f); // back face, triangle 1
    triN(x0, y0, zb,  x0, y1, zb,  x1, y1, zb,   0.0f, 0.0f, -1.0f); // back face, triangle 2

    // Bottom wall (the y = y0 side), normal points straight down: (0,-1,0).
    triN(x0, y0, zf,  x1, y0, zb,  x1, y0, zf,   0.0f, -1.0f, 0.0f); // bottom wall, triangle 1
    triN(x0, y0, zf,  x0, y0, zb,  x1, y0, zb,   0.0f, -1.0f, 0.0f); // bottom wall, triangle 2

    // Top wall (the y = y1 side), normal points straight up: (0,1,0).
    triN(x0, y1, zf,  x1, y1, zf,  x1, y1, zb,   0.0f, 1.0f, 0.0f); // top wall, triangle 1
    triN(x0, y1, zf,  x1, y1, zb,  x0, y1, zb,   0.0f, 1.0f, 0.0f); // top wall, triangle 2

    // Left wall (the x = x0 side), normal points in -x: (-1,0,0).
    triN(x0, y0, zf,  x0, y1, zb,  x0, y1, zf,   -1.0f, 0.0f, 0.0f); // left wall, triangle 1
    triN(x0, y0, zf,  x0, y0, zb,  x0, y1, zb,   -1.0f, 0.0f, 0.0f); // left wall, triangle 2

    // Right wall (the x = x1 side), normal points in +x: (1,0,0).
    triN(x1, y0, zf,  x1, y1, zf,  x1, y1, zb,   1.0f, 0.0f, 0.0f); // right wall, triangle 1
    triN(x1, y0, zf,  x1, y1, zb,  x1, y0, zb,   1.0f, 0.0f, 0.0f); // right wall, triangle 2
}

// Builds the full solid-3D vertex list for one letter by looking up its
// strokes in kLetterRects and extruding each one. No index buffer (EBO)
// here on purpose -- keeping this as plain glDrawArrays(GL_TRIANGLES, ...)
// keeps the demo's draw call as close as possible to what you saw for a
// single triangle in lecture. You'll see indexed drawing (EBOs) in
// Assignment 2 and beyond, where reusing shared vertices actually matters.
// Each vertex in the returned list is (position, normal) = 6 floats.

std::vector<float> buildLetterVertices(char ch) {
    std::vector<float> verts;                    // will accumulate all triangles (as position+normal floats) for this one letter
    auto it = kLetterRects.find(ch);              // look up this character's stroke list in the table above
    if (it == kLetterRects.end()) {               // character not found -- unsupported letter/symbol
        std::cerr << "Letter " << ch << " not defined!" << std::endl; // report it on stderr
        return verts;                              // return an empty vertex list -- caller will skip drawing anything for this char
    }
    for (const Rect& r : it->second) {             // for every stroke that makes up this letter
        appendExtrudedRect(verts, r, kHalfDepth);   // extrude it into a solid box and append its 12 triangles to verts
    }
    return verts;                                   // hand the finished vertex list back to the caller
}

/* *************************************************** */
// =============================================================================
// SHADER COMPILE HELPERS
// =============================================================================
// Shaders are compiled AT RUNTIME, by the graphics driver, from the plain
// text strings above. That means typos in GLSL don't show up as a normal
// C++ compile error -- they show up here, printed to the console, when the
// program runs. ALWAYS check compile/link status like this. When you get a
// blank screen later in the semester, this is almost always where the
// answer is -- read the console output before you touch anything else.

unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);       // ask OpenGL for a new, empty shader object of this type
    glShaderSource(shader, 1, &source, nullptr);       // hand it our GLSL source text (1 string, no explicit length)
    glCompileShader(shader);                           // ask the driver to compile it

    int success;                                       // will hold GL_TRUE/GL_FALSE after the check below
    char infoLog[512];                                  // buffer to hold any compiler error message
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // ask OpenGL: did it compile successfully?
    if (!success) {                                     // it didn't --
        glGetShaderInfoLog(shader, 512, nullptr, infoLog); // ask the driver *why not*, into infoLog
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl; // print the reason
    }
    return shader; // an OpenGL "name" (really just an integer ID), not a C++ object
}

unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);      // compile the vertex shader
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc); // compile the fragment shader

    // A "program" is what you actually glUseProgram() and draw with -- it's
    // the vertex and fragment shaders linked together into one pipeline.
    unsigned int program = glCreateProgram();   // ask OpenGL for a new, empty program object
    glAttachShader(program, vertexShader);      // attach the compiled vertex shader to it
    glAttachShader(program, fragmentShader);    // attach the compiled fragment shader to it
    glLinkProgram(program);                     // ask the driver to link them together into one pipeline

    int success;                                            // will hold GL_TRUE/GL_FALSE after the check below
    char infoLog[512];                                       // buffer to hold any linker error message
    glGetProgramiv(program, GL_LINK_STATUS, &success);       // ask OpenGL: did linking succeed?
    if (!success) {                                          // it didn't --
        glGetProgramInfoLog(program, 512, nullptr, infoLog); // ask the driver *why not*, into infoLog
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl; // print the reason
    }

    // Once a shader is linked into a program, the individual shader objects
    // aren't needed anymore -- free them.
    glDeleteShader(vertexShader);   // the vertex shader's own object is no longer needed once linked
    glDeleteShader(fragmentShader); // same for the fragment shader
    return program; // the ID you'll pass to glUseProgram() before drawing
}

/* *************************************************** */
// =============================================================================
// PER-LETTER GPU BUFFERS
// =============================================================================
// A VBO (Vertex Buffer Object) is a block of GPU memory holding raw vertex
// data. A VAO (Vertex Array Object) records HOW to interpret that memory --
// which floats are the position, which are the normal, how far apart each
// vertex is (the "stride"), etc. -- so that later we can just bind the VAO
// and draw, without re-describing the layout every time.
//
// These are built ONCE at startup, not every frame. An earlier version of
// this demo created and destroyed a VAO/VBO every frame for every letter --
// that's wasted GPU churn 60 times a second for geometry that never
// changes, and it's not a habit worth picking up.

struct LetterBuffer {
    unsigned int VAO = 0;      // this letter's Vertex Array Object ID (0 = not created)
    unsigned int VBO = 0;      // this letter's Vertex Buffer Object ID (0 = not created)
    int vertexCount = 0;       // how many vertices to draw (space/undefined chars have 0)
};

// Maps each character we've already built a buffer for to that buffer.
// Repeated letters (e.g. the two A's in "GARCIA") end up sharing one entry
// -- we only need the geometry once, even though we'll draw it twice with
// two different transforms.

std::unordered_map<char, LetterBuffer> letterBuffers; // cache: character -> its GPU buffer info

void createLetterBuffer(char ch) {
    if (letterBuffers.count(ch)) return; // already built for this character -- nothing to do

    std::vector<float> verts = buildLetterVertices(ch);        // build this letter's CPU-side triangle list
    LetterBuffer buf;                                            // will hold the new VAO/VBO/count for this letter
    buf.vertexCount = static_cast<int>(verts.size() / 6);       // 6 floats/vertex (position+normal), so divide to get vertex count

    if (buf.vertexCount > 0) {                                   // only allocate GPU objects if this letter actually has geometry
        glGenVertexArrays(1, &buf.VAO);   // ask OpenGL for a new VAO name
        glGenBuffers(1, &buf.VBO);        // ask OpenGL for a new VBO name

        glBindVertexArray(buf.VAO);                          // "everything below configures THIS VAO"
        glBindBuffer(GL_ARRAY_BUFFER, buf.VBO);               // "the VBO we're about to describe/fill"
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
        // ^ copies our CPU-side `verts` vector into GPU memory. GL_STATIC_DRAW
        //   is a hint to the driver: "this data won't change often," which lets
        //   it choose faster storage than if we were rewriting it every frame.

        // Tell the VAO: attribute 0 (aPos in the vertex shader) is 3 floats,
        // starting at byte offset 0, and consecutive vertices are 6 floats
        // (24 bytes) apart.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // describe attribute 0 (aPos): 3 floats, offset 0, stride 6 floats
        glEnableVertexAttribArray(0); // turn attribute 0 on so the GPU actually reads it

        // Attribute 1 (aNormal) is also 3 floats, but starts 3 floats (12
        // bytes) into each vertex -- right after the position.
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // describe attribute 1 (aNormal): 3 floats, offset 3 floats in
        glEnableVertexAttribArray(1); // turn attribute 1 on so the GPU actually reads it

        glBindVertexArray(0); // unbind -- good hygiene, avoids accidentally editing this VAO later
    }

    letterBuffers[ch] = buf; // store this letter's (possibly empty) buffer info in the cache
}

/* *************************************************** */
// =============================================================================
// INPUT HANDLING
// =============================================================================
// GLFW gives you two different ways to read the keyboard, and this demo
// uses both so you can see when each one is the right tool:
//
//   1. POLLING (see processInput(), called once per frame from the render
//      loop): you ask "is this key down RIGHT NOW?" every frame. This is
//      the right choice for anything that should keep changing smoothly for
//      as long as a key is held -- e.g. moving a camera while WASD is held.
//      ESC (quit) is the one control left using it here.
//
//   2. CALLBACKS (see key_callback() below, registered once with
//      glfwSetKeyCallback): GLFW calls your function whenever a key's state
//      CHANGES (pressed, released, or auto-repeated). This is the right
//      choice for a one-shot action that should happen exactly once per
//      press -- pause, reset, and stepping to the next/previous speed LEVEL
//      all fit this, and all use it below. (An earlier version of this file
//      drove the speed control through polling instead, incrementing it a
//      little every single frame a key was held -- which is exactly why it
//      used to feel like it went from a full stop to blazing fast almost
//      instantly. Polling is the wrong tool once what you want is "step
//      through a short list of fixed levels, once per press.")
//
// Controls in this demo:
//   ESC              quit                                  (polling)
//   SPACE            pause / resume the spin                (callback, GLFW_PRESS only)
//   R                reset every letter's rotation to 0     (callback, GLFW_PRESS only)
//   UP / DOWN        step to the next / previous speed level (callback, GLFW_PRESS only)
//   + / -            same as UP / DOWN -- a second set of keys bound to the same action

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // is ESC currently held down, this very frame?
        glfwSetWindowShouldClose(window, true);            // yes -- flag the window to close; the render loop checks this next iteration
}

// Called by GLFW whenever a key is pressed, released, or repeated (see
// glfwSetKeyCallback in main()). We only act on `action == GLFW_PRESS`,
// precisely so holding a key down doesn't repeatedly fire these.

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)window;   // unused parameter -- (void)ing it silences "unused parameter" warnings
    (void)scancode; // a platform-specific code we don't need; `key` (a GLFW_KEY_* constant) is enough
    (void)mods;     // modifier keys (shift/ctrl/alt) held during the press -- not used here

    if (action != GLFW_PRESS) {
        return; // ignore GLFW_RELEASE and GLFW_REPEAT -- every action below is one-shot
    }

    if (key == GLFW_KEY_SPACE) {
        paused = !paused; // flip the boolean: one press = one toggle, no matter how long it's held
    }

    if (key == GLFW_KEY_R) {
        // Reset every letter's spin back to the start. This is a good
        // example of a callback driving a change to state that the render
        // loop reads later -- key_callback never touches the screen itself.
        for (float& angle : rotations) { // walk every letter's accumulated angle, by reference so we can modify it
            angle = 0.0f;                // snap it back to zero
        }
    }

    // Speed levels: each press moves ONE step through kSpeedLevels, clamped
    // to the array's bounds so it can't index off either end. GLFW_KEY_EQUAL
    // is the '=' / '+' key (GLFW key codes identify the physical key, not
    // whether Shift is held) and GLFW_KEY_KP_ADD is '+' on the numeric
    // keypad -- binding both means the control works whether or not the
    // keyboard even has a keypad. Same idea for MINUS / KP_SUBTRACT.
    bool speedUpPressed = (key == GLFW_KEY_UP || key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD);       // was a "speed up" key pressed?
    bool slowDownPressed = (key == GLFW_KEY_DOWN || key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT); // was a "slow down" key pressed?

    if (speedUpPressed && speedLevelIndex < static_cast<int>(kSpeedLevels.size()) - 1) { // only step up if not already at the fastest level
        speedLevelIndex++;                          // move to the next (faster) level
        rotationSpeed = kSpeedLevels[speedLevelIndex]; // read the actual speed value out of the table
    }
    if (slowDownPressed && speedLevelIndex > 0) { // only step down if not already at the slowest level
        speedLevelIndex--;                          // move to the previous (slower) level
        rotationSpeed = kSpeedLevels[speedLevelIndex]; // read the actual speed value out of the table
    }
}

/* *************************************************** */
// =============================================================================
// PLATFORM DETECTION (Linux/WSL only -- a no-op on Windows/macOS)
// =============================================================================
// On Linux, GLFW can talk to the window system through either the X11 or
// Wayland backend. Under WSLg (WSL's built-in display server), the Wayland
// backend has a known bug: a window's surface doesn't get resized when you
// maximize it or make it fullscreen, so the rendered image stays clipped to
// its old size while the window itself grows -- an ugly "frozen" artifact.
// The X11 backend doesn't have this bug, and WSLg supports both, so the fix
// is simply: under WSL specifically, ask GLFW to use X11 instead of letting
// it default to Wayland. Everywhere else (a real Linux desktop, Windows,
// macOS) we leave GLFW's own default alone, since the bug is a WSLg quirk,
// not a general Wayland problem.

// Returns true if this process is running inside Windows Subsystem for
// Linux. WSL sets one of a couple of environment variables in every
// process; if neither is present we fall back to checking /proc/version,
// which WSL's kernel always stamps with "microsoft".

bool isRunningUnderWSL() {
    if (std::getenv("WSL_DISTRO_NAME") != nullptr) return true; // set by WSL for every process -- fastest check first
    if (std::getenv("WSL_INTEROP") != nullptr) return true;     // another WSL-specific environment variable, as a backup

    std::ifstream versionFile("/proc/version"); // open the kernel version file (Linux-specific, always readable)
    if (versionFile) {                          // did it open successfully?
        std::string contents((std::istreambuf_iterator<char>(versionFile)), // read the ENTIRE file into one string...
                              std::istreambuf_iterator<char>());            // ...using the "range constructor" idiom
        for (char& c : contents) {              // walk every character in the file's contents, by reference
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c))); // lowercase it in place, for a case-insensitive search
        }
        if (contents.find("microsoft") != std::string::npos) return true; // WSL's kernel version string always contains "microsoft"
    }
    return false; // none of the WSL signals were present -- probably not running under WSL
}

/* *************************************************** */

int main() {
    // ---- Step 0: steer GLFW away from WSLg's buggy Wayland backend -----
    // This MUST happen before glfwInit() -- GLFW_PLATFORM is an init hint,
    // not something you can change once a window exists. GLFW_PLATFORM was
    // added in GLFW 3.4; the #if keeps this file building against older
    // GLFW too (it just silently skips the hint, so you'd still see the
    // resize artifact on an old GLFW under WSL, but everything else works).
#if defined(GLFW_VERSION_MAJOR) && (GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4))
    // glfwPlatformSupported() also needs glfwInit() to *not* have run yet,
    // so this whole check has to live right here. Belt-and-suspenders: only
    // force X11 if this GLFW build actually has X11 support compiled in.
	
    if (isRunningUnderWSL() && glfwPlatformSupported(GLFW_PLATFORM_X11)) { // are we under WSL, AND does this GLFW build support X11?
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);                    // yes to both -- tell GLFW to use X11 instead of its default
    }
#endif

    // ---- Step 1: create a window + OpenGL context via GLFW -------------
    if (!glfwInit()) {                                            // initialize the GLFW library itself
        std::cerr << "Failed to initialize GLFW" << std::endl;    // report failure
        return -1;                                                 // exit with a non-zero (error) status
    }

    // Ask for an OpenGL 3.3 Core Profile context -- "core" means no legacy
    // fixed-function features (glBegin/glEnd and friends); everything goes
    // through shaders, which is what we want for the rest of this course.
	
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                // request OpenGL major version 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                // request OpenGL minor version 3 (together: 3.3)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // request the Core (non-legacy) profile

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, titleString.c_str(), nullptr, nullptr); // actually create the window + GL context
    if (!window) {                                                          // did window creation fail? (e.g. unsupported GL version)
        std::cerr << "Failed to create GLFW window" << std::endl;          // report failure
        glfwTerminate();                                                    // clean up whatever GLFW state exists so far
        return -1;                                                          // exit with a non-zero (error) status
    }

    glfwMakeContextCurrent(window); // all following OpenGL calls apply to this window's context

    // Explicitly set a standard arrow cursor on the window. Under WSLg's
    // X11 (XWayland) backend in particular, a GL window's cursor sometimes
    // renders blank/invisible if you just leave it as whatever GLFW's
    // default happened to inherit from the system cursor theme -- asking
    // for a specific standard shape sidesteps that theme lookup and draws
    // GLFW's own cursor image instead, which is a known fix for "the mouse
    // disappears over the window" under WSLg. Harmless on other platforms.
	
    GLFWcursor* arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR); // create a standard system arrow cursor shape
    glfwSetCursor(window, arrowCursor);                                    // apply it to this window

    // Register our callbacks. GLFW will call these automatically -- we
    // never call them ourselves.
	
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // register: call this whenever the window is resized
    glfwSetKeyCallback(window, key_callback);                          // register: call this whenever a key changes state

    // ---- Step 2: load OpenGL function pointers via GLAD ----------------
    // GLFW only creates the *context*; it doesn't know which OpenGL
    // functions exist on this machine/driver. GLAD looks them up for us.
	
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {         // ask GLAD to load every OpenGL function pointer, via GLFW's loader
        std::cerr << "Failed to initialize GLAD" << std::endl;         // report failure
        return -1;                                                     // exit with a non-zero (error) status
    }

    // ---- Step 3: compile our shader program -----------------------------
	
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource); // compile + link our one shader program

    // Enable depth testing -- now that letters are solid 3D boxes instead
    // of flat coplanar quads, this is what makes nearer surfaces correctly
    // hide farther ones instead of fighting for the same pixels.
	
    glEnable(GL_DEPTH_TEST); // turn on depth testing

    // ---- Step 4: prepare the string to display --------------------------
    // Uppercase everything, since that's the only case kLetterRects defines
    // shapes for.
	
    for (char& c : displayString) {                                             // walk every character in the string, by reference so we can modify it in place
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));    // force it to uppercase
    }

    // Work out how to lay the letters out left-to-right so the whole
    // string fits across the screen (roughly -1..+1 in normalized
    // coordinates), no matter how long displayString is.
	
    float spacing = 2.0f / displayString.length();                 // horizontal gap between letter centers, so the row spans about [-1, 1]
    float offset = -1.0f + (spacing / 2.0f);                       // x-position of the very first letter's center
    float scale = std::min(0.5f, 1.0f / displayString.length());   // shrink letters if the string is long; never bigger than 0.5

    // One rotation angle and one color offset per character.
	
    rotations.resize(displayString.length(), 0.0f);   // one rotation angle per letter, all starting at 0 degrees
    colorOffsets.resize(displayString.length());        // one color-cycle phase offset per letter (values filled in below)
    for (size_t i = 0; i < colorOffsets.size(); ++i) {  // for every letter index
        colorOffsets[i] = static_cast<float>(i);        // stagger this letter's color cycle by its position in the string
    }

    // ---- Step 5: build GPU geometry for every letter we'll need ---------
    // Doing this once, up front, is why the render loop below never has to
    // call glGenBuffers/glBufferData -- it just binds and draws.
	
    for (char c : displayString) {  // for every character that will appear on screen
        createLetterBuffer(c);      // build (or reuse, if already built) its VAO/VBO
    }

    // ---- Step 6: the render loop -----------------------------------------
    // This runs once per frame until the user closes the window. Everything
    // that changes over time (rotation, color, whether we're paused) is
    // recomputed here; everything that doesn't change (the geometry itself)
    // was already uploaded to the GPU in Step 5.
	
    while (!glfwWindowShouldClose(window)) {                     // keep looping until the user closes the window (or ESC sets this flag)
        processInput(window);                                     // poll continuously-held keys (just ESC now)

        // Clear both the color buffer (the visible image) and the depth
        // buffer (per-pixel "distance from camera" used by depth testing)
        // before drawing this frame's contents.
		
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);                              // set the "clear to" color: a dark gray background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                 // actually clear the color buffer and depth buffer for this frame

        glUseProgram(shaderProgram); // "use this shader program for every draw call below"

        for (size_t i = 0; i < displayString.length(); ++i) {   // draw one letter at a time, left to right
            char letter = displayString[i];                       // this letter's character
            const LetterBuffer& buf = letterBuffers[letter];       // its precomputed VAO/VBO/vertexCount
            if (buf.vertexCount == 0) {                             // space, or an unsupported character
                continue;                                            // nothing to draw -- skip straight to the next letter
            }

            // ---- Build this letter's transform matrix ----
            // This is written out by hand and uploaded row-major (note the
            // GL_TRUE below, which tells OpenGL to transpose it for us) so
            // you can see exactly what a rotation+scale+translate matrix
            // looks like as plain numbers. From Assignment 2 onward we use
            // GLM to build matrices like this instead of writing them by
            // hand -- but GLM is doing exactly this kind of math for you.
			
            float angle = rotations[i] * (float)M_PI / 180.0f; // convert this letter's current rotation from degrees to radians
            float cosA = cos(angle);                            // cosine of the rotation angle, reused below in the matrix
            float sinA = sin(angle);                            // sine of the rotation angle, reused below in the matrix

            // This matrix combines three things at once: rotate around Y by
            // `angle`, scale uniformly by `scale`, then translate along X so
            // letter i lands in its spot in the string.
			
            float transform[16] = {
                scale * cosA, 0.0f, scale * sinA, offset + (i * spacing), // row 0: x-axis basis (rotate+scale) and this letter's x position
                0.0f, scale, 0.0f, 0.0f,                                  // row 1: y-axis basis (scaled only -- Y rotation doesn't touch Y)
                -scale * sinA, 0.0f, scale * cosA, 0.0f,                  // row 2: z-axis basis (rotate+scale)
                0.0f, 0.0f, 0.0f, 1.0f                                    // row 3: homogeneous row, unchanged for this kind of transform
            };

            int transformLoc = glGetUniformLocation(shaderProgram, "transform"); // ask the shader program where its "transform" uniform lives
            glUniformMatrix4fv(transformLoc, 1, GL_TRUE, transform);              // upload it -- GL_TRUE transposes, since we wrote it row-major above

            // ---- Compute this letter's color ----
            // Three sine waves, 120 degrees out of phase with each other
            // (the +2.0 / +4.0 offsets), sweep smoothly through a rainbow
            // of colors over time. colorOffsets[i] shifts each letter's
            // wave so they don't all flash the same color simultaneously.
			
            float red = (sin(glfwGetTime() + colorOffsets[i]) + 1.0f) / 2.0f;            // red channel, oscillating between 0 and 1 over time
            float green = (sin(glfwGetTime() + colorOffsets[i] + 2.0f) + 1.0f) / 2.0f;   // green channel, phase-shifted from red
            float blue = (sin(glfwGetTime() + colorOffsets[i] + 4.0f) + 1.0f) / 2.0f;    // blue channel, phase-shifted from both
            int colorLoc = glGetUniformLocation(shaderProgram, "color"); // ask the shader program where its "color" uniform lives
            glUniform3f(colorLoc, red, green, blue);                     // upload this frame's color for this letter

            // ---- Draw ----
			
            glBindVertexArray(buf.VAO);                       // "use this letter's geometry for the next draw call"
            glDrawArrays(GL_TRIANGLES, 0, buf.vertexCount);   // issue the actual draw call -- draws buf.vertexCount vertices as triangles

            // ---- Advance this letter's rotation for next frame ----
            // Only when not paused -- this is where the `paused` flag set
            // by key_callback(), and the `rotationSpeed` adjusted by
            // key_callback(), actually take effect.
			
            if (!paused) {                          // skip advancing rotation entirely while paused
                rotations[i] += rotationSpeed;       // add this frame's increment to the accumulated angle, for next frame
            }
        }

        glfwSwapBuffers(window); // show the frame we just drew (we draw to an offscreen buffer, then swap)
        glfwPollEvents();        // let GLFW process OS events (keyboard, window close, etc.) -- this is
                                 // also what actually triggers key_callback/framebuffer_size_callback
    }

    // ---- Cleanup -----------------------------------------------------------
    // Every buffer we created once in Step 5 gets deleted once here. OpenGL
    // doesn't garbage-collect for you -- GPU resources you Gen(erate), you
    // must Delete.
	
    for (auto& kv : letterBuffers) {           // walk every letter buffer we ever created
        LetterBuffer& buf = kv.second;         // this entry's VAO/VBO/vertexCount
        if (buf.vertexCount > 0) {             // only delete buffers that were actually allocated (skip space's empty entry)
            glDeleteVertexArrays(1, &buf.VAO); // free the VAO on the GPU
            glDeleteBuffers(1, &buf.VBO);      // free the VBO on the GPU
        }
    }
    glDeleteProgram(shaderProgram); // free the compiled/linked shader program
    glfwDestroyCursor(arrowCursor); // free the cursor object we created earlier
    glfwTerminate();                // shut down GLFW entirely -- closes the window, frees all remaining GLFW state
    return 0;                       // exit the program successfully
}

// Called every time the window is resized (by GLFW, via the callback we
// registered in main()). We just need to tell OpenGL that the drawable area
// changed size, so it maps clip-space coordinates to the new pixel
// dimensions correctly.

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;                        // unused parameter -- silence the "unused parameter" warning
    glViewport(0, 0, width, height);     // tell OpenGL the new pixel size of the drawable area
}
