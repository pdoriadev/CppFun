// Last LearnOpenGL page - https://learnopengl.com/Getting-started/Hello-Window
// GLSL Data Type page - https://wikis.khronos.org/opengl/Data_Type_(GLSL) 

//-/////////////////////////////////////////////////////////////////////
// BUILDING THIS PROGRAM FROM COMMAND LINE
// Use g++ since we're using Cpp.
// Add main.cpp and glad.c as source targets. Add any other source targets. 
// -o param to create objects
// executable name
// link GLAD and GLFW libraries
// -lglfw -lGL -ldl     
//      -lglfw - ?? links GLFW ??
//      -lGL ?? links OpenGL ??
//      -ldl ?? what does this link ??

#pragma region HEADERS

// OPENGL-RELATED HEADERS
#include "glad.h"           // Function pointers to hardware implementation of OpenGL. Must include before GLFW
#include <GLFW/glfw3.h>     // Window + Input Library - window/context creation, input, timing
                            // HTML Documentation - https://www.glfw.org/docs/latest/
// C / CPP HEADERS
#include <cctype>           // std::toupper, std::tolower
#include <cstdlib>          // std::getenv -- used by isRunningUnderWSL() below
#include <fstream>          // std::ifstream -- used by isRunningUnderWSL() below
#include <string>           // std::string - used by isRunningUnderWSL() below
// My C / CPP HEADERS
#include "Logging.h"

#pragma endregion

#pragma region PROTOYTPES

// Is this a CPP prototype??? Cuz the naming is C-style??
// GLFW prototypes
void framebuffer_size_callback(GLFWwindow*, int, int);

// CPP Prototypes
bool setupWSL();
bool isRunningUnderWSL();
bool InitStep1();
bool InitStep2_WindowAndViewport(GLFWwindow* window_close);;
bool processInput(GLFWwindow*);

static const bool IsNullPtr(void*, const std::string);
#pragma endregion

#pragma region FIELDS
enum Platform
{
    UNKNOWN = -1,
    LINUX = 0
    WSL = 1
};
Platform platform = Platform.UNKNOWN;

const std::string DASH_LINE = "--------------------------";
#pragma endregion

#pragma region SHADER_SOURCE_STRUCTS_FUNCTIONS
const char* vertexShaderSource =  R"GLSL(
// OpenGL version to run the shader
#version 330 core
// layout?? location??
// in is input vector data.
layout (location = 0) in vec3 a;

void main()
{
    // gl_Position is a predefined variable. 
    //      ?? Used for each vertex that passes through the vertex shader??
    gl_Position(a.X, a.Y, a.Z, 1.0f);
})GLSL";

const char* fragmentShaderSource = R"GLSL(
#version 330 core
out vec4 fragColor;
void main()
{
    fragColor = vec4(0.2f, 1f, 0.5f, 1.0f);
})GLSL"

struct CompileShaderParams
{
    bool isInitialized = false;
    unsigned int shaderType;
    const char* ptrToShaderSource;
    unsigned int& refToOutShader;

    CompileShaderParams(unsigned int _shaderType, 
        const char* _ptrToShaderSource,
        unsigned int& _refToOutShader)
    {
        switch(_shaderType)
        {
            case GL_VERTEX_SHADER:
                break;
            case GL_FRAGMENT_SHADER:
                break;
            default:
                ConsoleLog(Logging::LogType::ASSERT, _shaderType + " does not match a valid shader type. See: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateShader.xhtml");
                return;
        }

        // TODO Needs validation. Skipping for now. 
        ptrToShaderSource = _ptrToShaderSource;

        // Do I need validation here??
        refToOutShader = _refToOutShader;

        isInitialized = true;
    }
};

bool compileShader(CompileShaderParams params)
{
    // Create a shader object of given type. Returns the object's id.
    params.refToOutShader = glCreateShader(params.shaderType);
    if (refToOutShader == 0)
    {
        // TODO - add additional info for the shadersource. etc.
        Logging::ConsoleLog(Logging::LogType::ASSERT, "ERROR: glCreateShader returned 0." + \
                                                    "\nShader Type: " + params.shaderType);
    }
    // use strcat to concatenate the char * with the string. 

    // Replace shader object's source code
    // param 1 - shader id number.
    // param 2 - number of elements in the string and length arrays
    // param 3 - const char**. shader source code
    // param 4 - length of source code string. nullptr - no explicit length
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glShaderSource.xhtml 
    glShaderSource(params.refToOutShader, 1, &params.ptrToShaderSource, NULL);

    glCompileShader(params.refToOutShader);

    // Copied and updated from Assignment_0 
    int success;                                       // will hold GL_TRUE/GL_FALSE after the check below
    char infoLog[512];                                  // buffer to hold any compiler error message
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // ask OpenGL: did it compile successfully?
    if (!success) {                                     // it didn't --
        glGetShaderInfoLog(shader, 512, nullptr, infoLog); // ask the driver *why not*, into infoLog
        Logging::ConsoleLog(Logging::LogType::ASSERT, "ERROR::SHADER::COMPILATION_FAILED\n" + infoLog); // print the reason
        return false;
    }

    return true;
}

bool setupShaderProgram()
{
    // Create vertex shader object
    unsigned int& vertexShaderID;
    {
        CompileShaderParams vertexParams = CompileShaderParams(
            GL_VERTEX_SHADER, 
            vertexShaderSource,
            vertexShaderID);
        compileShader(vertexParams);
    }

    // Create frag shader object
    unsigned int& fragmentShaderID;
    {
        CompileShaderParams fragParams = CompileShaderParams(
            GL_FRAGMENT_SHADER,
            fragmentShaderSource,
            fragmentShaderID);
        compileShader(fragmentShaderID);
    }

    // Create program object. 
    // Attach shader objects to program.
    // Link program object. Create an executable for each shader type (i.e. vert, frag, etc.).
    //      Log linking results.
    // Add linked program object to current rendering context. 
    unsigned int shaderProgramID;
    {
        //-//////////////////////////
        // glCreateProgram()
        // 
        // creates an empty shader program. Returns the program's ID. 
        // returns - void
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateProgram.xhtml
        shaderProgramID = glCreateProgram();
        
        //-//////////////////////////
        // glAttachShader(unsigned int, unsigned int)
        // param 1 - program ID
        // param 2 - a shader object ID (i.e. frag, vert, etc.) 
        // returns - void
        //
        // Attaches a shader object to a program. Programs link shader objects together.
        // 
        // Permissible to attach a shader object before source code is loaded or before it is compiled.
        // Can attach multiple shader objects of the same type. 
        // When an attached shader object is deleted, call glDetachShader to detach from programs.
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glAttachShader.xhtml
        glAttachShader(shaderProgramID, vertexShaderID);
        glAttachShader(shaderProgramID, fragmentShaderID);

        //-//////////////////////////
        // glLinkProgram()
        // param 1 - program ID. 
        //
        // Create an executable for each set of shaders (vert, geo, frag, etc.) attached to a program.
        // The exectuable will run on that shader's corresponding processor (vert on vertex, geo on geomoetry, etc.)
        // On successful link
        //      - the program's active user-defined ?? uniform ?? variables are initialized to 0. 
        //      - the program's active ?? uniform ?? variables are assigned a location. 
        //          - the location can be queried by ?? glGetUniformLocation ??. 
        //      - ?? Unbound active user-defined variables are bound to a generic vertex attribute index ??
        //      - Changes to shader objects do not affect program exectuables. 
        // On failed link
        //      - link status is set to GL_FALSE. 
        //      - If this is a *re-link*, exectuables remain part of current state. Call glUseProgram to remove them. 
        // NOTE: Always include a frag shader. Not including one leads to undefined behavior.
        //
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glLinkProgram.xhtml
        //      - includes list of why programs fail to link. 
        glLinkProgram(shaderProgramID);

        //-///////////////////////////
        // void glGetProgramInfoLog()
        // param 1 - unsigned int - program ID
        // param 2 - size_t - max length of log in character buffer
        // param 3 - size_t - actual length of log, excluding the null terminator character.
        //                  - Can pass in NULL if the length is not required. 
        //                  - Can get actual length by passing GL_INFO_LOG_LENGTH into glGetProgram.
        // param 4 - char* - character buffer to copy log into.
        // 
        // Copies a program's info log into a buffer up to a max length. 
        // - Info log is updated when a program is linked or validated. 
        // - Info log is in one of three states. 
        //      - A - Empty
        //      - B - Info about the last link operation.
        //      - C - Info about the last validate operation.
        // - A program object's info log is empty at creation.
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetProgramInfoLog.xhtml
        char[2048] logBuffer;
        glGetProgramInfoLog(
            shaderProgramID, 
            2047,
            NULL,
            logBuffer);
        Logging::ConsoleLog(Logging::LogType::LOG, "PROGRAM INFO LOG AFTER LINKING:\n".c_str());
        Logging::ConsoleLog(Logging::LogType::LOG, logBuffer);
        Logging::ConsoleLog(Logging::LogType::LOG, "\nEND OF PROGRAM INFO LOG\n");

        //-//////////////////////////
        // glUseProgram()
        // Installs a program object as part of current rendering state
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUseProgram.xhtml 
        glUseProgram(shaderProgramID);
    }

    // Clean-up
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return true;
}



#pragma endregion

int main()
{
    Logging::ConsoleLog(Logging::LogType::LOG, \
        ("STARTING PROGRAM\n" + DASH_LINE + "\n").c_str());

    //-//////////////////////////////////////////////////////////////
    // PLATFORM SETUP, GLFW SETUP
    //-//////////////////////////////////////////////////////////////

    InitStep1();

    //-//////////////////////
    // glfwCreateWindow call. - https://www.glfw.org/docs/latest/group__window.html#ga3555a418df92ad53f917597fe2f64aeb
    //
    // param 1 - width (columns)
    // param 2 - height (rows)
    // param 3 - window name
    // param 4 - ? 
    // param 5 - ?
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (IsNullPtr(window, "GLFWwindow"))
    {
        Logging::ConsoleLog(Logging::LogType::ASSERT,
            (DASH_LINE + "\nFailed to create GLFW window\n" + DASH_LINE).c_str());
        glfwTerminate();
        return -1;
    }

    InitStep2_WindowAndViewport(window);

    //-//////////////////////////
    // glfwMakeContextCurrent()
    // Makes this window the only window current on the calling thread.
    // https://www.glfw.org/docs/latest/group__context.html#ga1c04dc242268f827290fe40aa1c91157
    //
    // I'm not sure I need this??? It worked before having this call?
    // glfwMakeContextCurrent(window);
    
    //-//////////////////////////
    // Disappearing arrow cursor fix for X11 + WSL platform. 
    // Copied from Assignment_0
    GLFWcursor* arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR); // create a standard system arrow cursor shape
    glfwSetCursor(window, arrowCursor);                                    // apply it to this window

    //-//////////////////////////////////////////////////////////////
    // SHADER SETUP
    //-//////////////////////////////////////////////////////////////
    
    setupShaderProgram();
    
    //-//////////////////////////////////////////////////////////////
    // RENDER LOOP
    //-//////////////////////////////////////////////////////////////

    // glfwWindowShouldClose() call
    // - returns a flag. If true, do we close the window manually???? Or does glfw handle that??
    // ?? how is the flag set/determined ??
    while (glfwWindowShouldClose(window) == false)
    {
        

        processInput(window);

        // glfwSwapBuffers call
        // swaps the new buffer to the screen.
        // ?? waits until fully drawn ??
        glfwSwapBuffers(window);
        // glfwPollEvents call
        // Checks for inputs. 
        glfwPollEvents();
    }

    //-//////////////////////////////////////////////////////////////
    // CLEAN-UP - clean/delete allocated GLFW resources
    //-//////////////////////////////////////////////////////////////

     
    // glfwTerminate()
    //      - Destroys remaining windows
    //      - Frees allocated resources
    //      - Sets library data to an uninitialized state
    // After calling glfwTerminate(), call glfwInit to use GLFW functions again.
    // Call glfw whenever exiting the render loop
    //      ?? What if there are multiple viewports / windows ??
    //      ?? Will terminating kill those viwports / windows, too? ?? 
    // 
    glfwTerminate();

    Logging::ConsoleLog(Logging::LogType::LOG,
        (DASH_LINE + "\nENDING PROGRAM\n").c_str());

    return 0;
}

#pragma region INITIALIZATION

//-//////////////////////////////////////////////////////////////
// WSL
// WindowHints 
//      (OpenGL version for GLFW to use)
//      CORE or COMPATIBILITY profile
// Init glfw
bool InitStep1()
{
    //-//////////////////////////////////////////////////////////////////////
    // 
    bool setupForWSL = setupWSL();
    std::string setupResultString = setupForWSL ? "TRUE" : "FALSE";
    Logging::ConsoleLog(Logging::LogType::LOG,
        ("SETUP FOR WSL: " + setupResultString + "\n").c_str());

    //-////////////////////////////////////////////////////////////////////
    // glfwWindowHint call - sets data for hints for next glfwCreateWindow call. 
    // - param 1 - select option from possible options prefixed with GLFW_.
    //      - full list of options found here: http://www.glfw.org/docs/latest/window.html#window_hints
    // - param 2 - integer. sets the value of our option. 
    //
    // Together, sets to OPENGL version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // CORE_PROFILE - the current set of features. 
    // COMPATITIBILITY_PROFILE would expose backwards-compatible features: https://community.khronos.org/t/opengl-core-profile-and-opengl-compat-profile/108643
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Initialize GLFW
    if (glfwInit())
    {
        Logging::ConsoleLog(Logging::LogType::LOG,
            ("INITIALIZED: TRUE\n"));
    }
    else
    {
        Logging::ConsoleLog(Logging::LogType::ASSERT,
            ("INITIALIZED: FALSE\n" + DASH_LINE).c_str());
        return false;
    }

    return true;
}

bool InitStep2_WindowAndViewport(GLFWwindow* window)
{
    // make the created window the current context. 
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    // pass the OS-specific address of the OpenGL function pointers.
    // glfwGetProcAddress - defines the correct function based on which OS we're compiling for. 
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false)
    {
        Logging::ConsoleLog(Logging::LogType::ASSERT,
            (DASH_LINE + "\nFailed to initialize GLAD\n" + DASH_LINE).c_str());
        return false;
    }   

    //-/////////////////////////////////////////////////////////////////////////
    // glViewPort - Tell OpenGL the size of rendering window
    // registry.khronos.org/OpenGL-Refpages/gl4/html/glViewport.xhtml
    // 
    // ?? is this initializing the viewport?? Modifies the context's window? 
    //
    // param 1 - left-most x position. Normalized --> Domain of [-1, 1]
    // param 2 - bottom-most y position. Normalized --> Domain of [-1, 1]
    // param 3 - viewport width. ?? Can it be greater than the window??
    //              - If width < 0, throws an error.
    // param 4 - viewport height ?? Can it be greater than the window??
    //              - If height < 0, throws an error.
    /*
    Behind the scenes OpenGL uses the data specified via glViewport to 
        transform the 2D coordinates it processed to coordinates on your screen. 
        For example, a processed point of location (-0.5,0.5) would (as its final 
        transformation) be mapped to (200,450) in screen coordinates. Note that 
        processed coordinates in OpenGL are between -1 and 1 so we effectively 
        map from the range (-1 to 1) to (0, 800) and (0, 600). 
    */
    //
    // The viewport dimensions can be smaller than the window. 
    glViewport(0, 0, 800, 600);
    
    return true;
}

#pragma endregion

#pragma region WSL_FUNCTIONS

//-//////////////////////////////////////////////////
// Called before any glfw calls, including glfwInit().
// Copied functionality from Assignment_0.
bool setupWSL()
{
    //-//////////////////////////////////////////////
    // WSL SETUP
    // ---- Step 0: steer GLFW away from WSLg's buggy Wayland backend -----
    // This MUST happen before glfwInit() -- GLFW_PLATFORM is an init hint,
    // not something you can change once a window exists. GLFW_PLATFORM was
    // added in GLFW 3.4; the #if keeps this file building against older
    // GLFW too (it just silently skips the hint, so you'd still see the
    // resize artifact on an old GLFW under WSL, but everything else works).
    //
    // ^ PROF'S NOTES ^

#if defined(GLFW_VERSION_MAJOR) && (GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4))
    // glfwPlatformSupported() also needs glfwInit() to *not* have run yet,
    // so this whole check has to live right here. Belt-and-suspenders: only
    // force X11 if this GLFW build actually has X11 support compiled in.
    // 
    // ^ PROF'S NOTES ^
	
    // are we under WSL, AND does this GLFW build support X11?
    if (isRunningUnderWSL())
    {
        platform = PLATFORM.WSL;
    }
    
    if (platform == PLATFORM.WSL && glfwPlatformSupported(GLFW_PLATFORM_X11)) 
    {                                                      
        // yes to both -- tell GLFW to use X11 instead of its default
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);  
        return true;
    }

    return false;
#endif
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
//
// Returns true if this process is running inside Windows Subsystem for
// Linux. WSL sets one of a couple of environment variables in every
// process; if neither is present we fall back to checking /proc/version,
// which WSL's kernel always stamps with "microsoft".
//
//  ^ PROF'S NOTES ^
bool isRunningUnderWSL() 
{
    // Check environment variables first
    if (std::getenv("WSL_DISTRO_NAME") != nullptr) return true; // set by WSL for every process -- fastest check first
    if (std::getenv("WSL_INTEROP") != nullptr) return true;     // another WSL-specific environment variable, as a backup

    // Check for "microsoft" stamp on the linux kernel version. 
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

#pragma endregion

#pragma region RENDER_LOOP_HELPERS

//-///////////////////////////////////////////
// Called in main()'s RENDER LOOP.
// 
bool processInput(GLFWwindow *window)
{
    if (IsNullPtr(window, "GLFWwindow")) return false;

    //-/////////////////////////////////////////////
    // glfwGetKey() - https://www.glfw.org/docs/latest/input_guide.html#input_key  
    // param 1 - GLFWwindow pointer.
    // param 2 - A keycode macro. Full list: https://www.glfw.org/docs/latest/group__keys.html 
    // 
    // returns a key action.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        //-//////////////////////////////////////// 
        // glfwSetWindowShouldClose() - https://www.glfw.org/docs/latest/group__window.html#ga49c449dde2a6f87d996f4daaa09d6708
        // Sets the close flag on the specified window. Can override the user, or signal the window should be closed.
        // param 1 - pointer to a GLFWwindow. 
        // param 2 - int. ?? Is passing a non-zero/one value undefined?
        // returns - void.
        // Closing and Close flag - Closing and close flag: https://www.glfw.org/docs/latest/window_guide.html#window_close
        glfwSetWindowShouldClose(window, true);
        return true;
    }

    return false;
}

#pragma endregion

//-////////////////////////////////////////////////////////////////////////
// framebuffer_size_callback
// 
// Called when the window is resized
// Also called when window is first displayed. 
// Interesting Note - For retina displays width and height will end up 
//      significantly higher than the original input values. 
//
// ?? How does glfw know to call this? 
//      How does it know main.cpp implements this function at compile time? ??
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // When window is resized, viewport should be resized.
    // Register a callback. 
    glViewport(0, 0, width, height);
}

//-///////////////////////////////////////////////////////////////////////
//
double color = 0;
double adder = 0.001f;
bool colorLoop()
{
    color += adder;
    if (color >= 1 || color < 0)
    {
        adder *= -1.0f;
    }

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClearColor.xhtml
    // Inputs colors for glClear to use when it clears and sets the color buffer.
    // A *state-setting* function
    glClearColor(color, color * 0.5f,    color * 0.5f, color * 0.5f);
    
    // Clears the on screen buffer. Sets its buffer values *?for next render?*
    // A *state-using* function
    glClear(GL_COLOR_BUFFER_BIT);

    
}

#pragma region UTIL_FUNCTIONS

//-///////////////////////////////////////////////
//
static const bool IsNullPtr(void* pointer, std::string typeStr)
{
    if (pointer == NULL)
    {
        Logging::ConsoleLog(Logging::LogType::ERROR, 
            ("Pointer of type " + typeStr + "is null").c_str());
        return true;
    }

    return false;
}

#pragma endregion