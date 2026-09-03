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
#include "glad.h"           // manages function pointers for OpenGL.  must include before GLFW
#include <GLFW/glfw3.h>     // window/context creation, input, timing
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

const std::string DASH_LINE = "--------------------------";

#pragma endregion

int main()
{
    Logging::ConsoleLog(Logging::LogType::LOG, \
        ("STARTING PROGRAM\n" + DASH_LINE + "\n").c_str());
        
    InitStep1();

    //-//////////////////////////////////////////////////////////////
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
    
    //-/////////////////////////////////////////////
    // RENDER LOOP
    //
    // glfwWindowShouldClose() call
    // - returns a flag. If true, do we close the window manually???? Or does glfw handle that??
    // ?? how is the flag set/determined ??
    double color = 0;
    while (glfwWindowShouldClose(window) == false)
    {
        color += 0.01;
        if (color >= 1)
        {
            color = 0;
        }
        glClearColor(color, color, color, color);
        // Are we only ever actively drawing/updating settings for the buffer we can't see?
        // Or actually, both buffers probably share the came COLOR_BUFFER, but only one is
        // updated at a time based on that data. 
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);

        // glfwSwapBuffers call
        // swaps the new buffer to the screen.
        // ?? waits until fully drawn ??
        glfwSwapBuffers(window);
        // glfwPollEvents call
        // Checks for inputs. 
        glfwPollEvents();
    }

    //-/////////////////////////////////////////////
    // CLEAN-UP - clean/delete allocated GLFW resources
    // 
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

//-///////////////////////////////////////////////////////
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

#pragma region WSL_FUNCTIONS

//-//////////////////////////////////////////////////
// Called before any glfw calls, including glfwInit().
// 
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
    if (isRunningUnderWSL() && glfwPlatformSupported(GLFW_PLATFORM_X11)) 
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