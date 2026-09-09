// Last LearnOpenGL page - https://learnopengl.com/Getting-started/Hello-Window
// GLSL Data Type page - https://wikis.khronos.org/opengl/Data_Type_(GLSL) 
// OpenGL Enums Cheatsheet - https://loshkinoleg.github.io/cheatsheets/OpenGL_Enums_Cheatsheet 

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

#include <cstddef>
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
#include <vector>
// My C / CPP HEADERS
#include "Logging.h"

#pragma endregion ================================

#pragma region PROTOYTPES

// Is this a CPP prototype??? Cuz the naming is C-style??
// GLFW prototypes
void framebuffer_size_callback(GLFWwindow*, int, int);

// CPP Prototypes
bool setupWSL();
bool isRunningUnderWSL();
bool InitStep1();
bool InitStep2_WindowAndViewport(GLFWwindow* window_close);;

bool logShaderProgramInfo(unsigned int shaderProgramID);
bool processInput(GLFWwindow*);

static const bool IsNullPtr(void*, const std::string);
#pragma endregion ================================

#pragma region FIELDS
enum Platform : int32_t
{
    UNKNOWN = -1,
    LINUX = 0,
    WSL = 1
};
Platform platform = Platform::UNKNOWN;

const std::string DASH_LINE = "--------------------------";
#pragma endregion ================================

#pragma region SHADER_SOURCE 

const char* vertexShaderSource =  R"GLSL(
#version 330 core
// OpenGL version to run the shader
// layout?? location??
// in is input vector data.
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;

// "out" variables are computed once per vertex here, then automatically
// interpolated across each triangle before the fragment shader below sees
// them (that interpolation step is called rasterization).

out vec3 Normal; // will be picked up by "in vec3 Normal" in the fragment shader

// A "uniform" is a value we set once per draw call from the CPU (see
// glUniformMatrix4fv in the render loop) that stays constant across every
// vertex/pixel of that draw call -- unlike aPos/aNormal, which are
// different for every vertex.

uniform mat4 transform;                 // this letter's combined rotate+scale+position matrix, set from the CPU

void main()
{
    // Note from Assignment_0
    // gl_Position is a special built-in output: OpenGL reads it to know
    // where this vertex lands on screen (in clip space).
    gl_Position = vec4(aPos, 1.0);
    gl_Normal = vec4(aNor, 1.0);
}
)GLSL";

/////////////////////////////////////////////////

const char* fragmentShaderSource = R"GLSL(
#version 330 core

out vec4 fragColor;
in vec3 Normal;
uniform vec3 color;      // this letter's current color, set from the CPU each frame

void main()
{
    fragColor = vec4(0.2, 1, 0.5, 1.0);
}
)GLSL";

#pragma endregion ================================

#pragma region SHADER_STRUCTS_FUNCTIONS

struct CompileShaderParams
{
    bool isInitialized = false;
    unsigned int shaderType;
    const char* ptrToShaderSource;
    unsigned int& refToShaderID;

    CompileShaderParams(unsigned int _shaderType, 
        const char* _ptrToShaderSource,
        unsigned int& _refToShaderID) 
        : refToShaderID(_refToShaderID) // explicitly initialize reference. https://stackoverflow.com/questions/19576458/constructor-for-must-explicitly-initialize-the-reference-member
    {
        switch(_shaderType)
        {
            case GL_VERTEX_SHADER:
                break;
            case GL_FRAGMENT_SHADER:
                break;
            default:
                Logging::consoleLog(Logging::LogType::ASSERT, 
                    std::to_string(_shaderType) + " does not match a valid shader type. See: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateShader.xhtml");
                shaderType = 0;
                ptrToShaderSource = NULL;
                isInitialized = false;
                return;
        }

        shaderType = _shaderType;

        // TODO Needs validation. Skipping for now. 
        ptrToShaderSource = _ptrToShaderSource;

        // Do I need validation here??
        refToShaderID = _refToShaderID;

        isInitialized = true;
    }
};

bool compileShader(CompileShaderParams params)
{
    // Create a shader object of given type. Returns the object's id.
    params.refToShaderID = glCreateShader(params.shaderType);
    if (params.refToShaderID == 0)
    {
        // TODO - add additional info for the shadersource. etc.
        Logging::consoleLog(Logging::LogType::ASSERT, 
            std::string("ERROR: glCreateShader returned 0.\n Shader Type: ") + std::to_string(params.shaderType));
    }
    // use strcat to concatenate the char * with the string. 

    // Replace shader object's source code
    // param 1 - shader id number.
    // param 2 - number of elements in the string and length arrays
    // param 3 - const char**. shader source code
    // param 4 - length of source code string. nullptr - no explicit length
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glShaderSource.xhtml 
    glShaderSource(params.refToShaderID, 1, &params.ptrToShaderSource, NULL);

    glCompileShader(params.refToShaderID);

    // Copied and updated from Assignment_0 
    int success;                                       // will hold GL_TRUE/GL_FALSE after the check below
    char infoLog[512];                                  // buffer to hold any compiler error message
    glGetShaderiv(params.refToShaderID, GL_COMPILE_STATUS, &success); // ask OpenGL: did it compile successfully?
    if (!success) {                                     // it didn't --
        glGetShaderInfoLog(params.refToShaderID, 512, nullptr, infoLog); // ask the driver *why not*, into infoLog
        Logging::consoleLog(Logging::LogType::ASSERT, std::string("ERROR::SHADER::COMPILATION_FAILED\n") + std::string(infoLog)); // print the reason
        return false;
    }

    return true;
}

bool setupShaderProgram()
{
    // Create vertex shader object
    unsigned int vertexShaderID;
    {
        CompileShaderParams vertexParams = CompileShaderParams(
            GL_VERTEX_SHADER, 
            vertexShaderSource,
            vertexShaderID);
        compileShader(vertexParams);
    }

    // Create frag shader object
    unsigned int fragmentShaderID;
    {
        CompileShaderParams fragParams = CompileShaderParams(
            GL_FRAGMENT_SHADER,
            fragmentShaderSource,
            fragmentShaderID);
        compileShader(fragParams);
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
        // creates an empty shader program object. Returns the program's ID. 
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

        logShaderProgramInfo(shaderProgramID);

        //-///////////////////////////
        // glGetProgramiv - get value of one of program object's data
        // param 1 - int. program ID
        // param 2 - GLenum. desired data.
        // param 3 - out pointer to return desired data value
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetProgram.xhtml
        // 
        // Check if the last link was successful.
        {
            int param;
            glGetProgramiv(shaderProgramID,
                GL_LINK_STATUS,
                &param);
            if (param == GL_FALSE) {
                Logging::consoleLog(Logging::LogType::ASSERT, "FAILED TO LINK SHADER PROGRAM. ID = " + std::to_string(shaderProgramID));
            }
            else {
                Logging::consoleLog(Logging::LogType::LOG, "LINK SUCCESSFUL! ID = " + std::to_string(shaderProgramID));
            }
        }

        //-//////////////////////////
        // glUseProgram()
        // Installs a program object as part of current rendering state
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUseProgram.xhtml 
        glUseProgram(shaderProgramID);
    }

    // Clean-up shader objects
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return true;
}

// Look into alternative math library for later: https://ggt.sourceforge.net/
class vector3
{
public:
    float vec_xyz[3];

    float x() { return vec_xyz[0]; }
    float y() { return vec_xyz[1]; }
    float z() { return vec_xyz[2]; }
    bool set_x(float _x) { vec_xyz[0] = _x; return true;}
    bool set_y(float _y) { vec_xyz[1] = _y; return true;}
    bool set_z(float _z) { vec_xyz[2] = _z; return true;}

    vector3()
    {
        vec_xyz[0] = 0.0;
        vec_xyz[1] = 0.0;
        vec_xyz[2] = 0.0;
    };

    vector3(float _x, float _y, float _z)
    {
        vec_xyz[0] = _x;
        vec_xyz[1] = _y;
        vec_xyz[2] = _z;
    };

    bool add_to_vector(std::vector<float>& out)
    {
        out.emplace_back(x());
        out.emplace_back(y());
        out.emplace_back(z());
        return true;
    }

    // static const vector3 zeroVector()
    // {
    //     static const vector3 = vector3(0, 0, 0);
    // }
    // static vector3 const zeroVector (0, 0, 0);
};

class tri
{
public:
    vector3 points[3];
    vector3 normal;

    tri(vector3 a, vector3 b, vector3 c, vector3 norm)
    {
        points[0] = a;
        points[1] = b;
        points[2] = c;
        normal = norm;
    }
};

bool addTri(std::vector<float>& out, tri t)
{
    for (unsigned int i = 0; i < 3; ++i)
    {
        t.points[i].add_to_vector(out);
        t.normal.add_to_vector(out);
    }
}

//-////////////////////////////////////
// Make the triangles for each face of the cube. 
bool makeCube(std::vector<float>& out)
{
    vector3 zero        (0.5, 0.5, -0.5);
    vector3 one         (-0.5, 0.5, -0.5);
    vector3 two         (-0.5, 0.5, 0.5);
    vector3 three       (0.5, 0.5, 0.5);
    vector3 four        (0.5, -0.5, 0.5);
    vector3 five        (-0.5, -0.5, 0.5);
    vector3 six         (-0.5, -0.5, -0.5);
    vector3 seven       (0.5, -0.5, -0.5);

    vector3 sideANorm   (1, 0, 0);
    vector3 topNorm     (0, 1, 0);
    vector3 frontNorm   (0, 0, 1);
    vector3 backNorm    (0, 0, -1);
    vector3 botNorm     (0, -1, 0);
    vector3 sideBNorm   (-1, 0, 0);

    // Back Face
    addTri(out, tri(zero, one, six, backNorm));
    addTri(out, tri(zero, six, seven, backNorm));

    // Top Face
    addTri(out, tri(zero, one, two, topNorm));
    addTri(out, tri(zero, two, three, topNorm));

    // Front Face
    addTri(out, tri(two, three, four, frontNorm));
    addTri(out, tri(two, four, five, frontNorm));

    // Bottom Face
    addTri(out, tri(four, five, six, botNorm));
    addTri(out, tri(four, six, seven, botNorm));

    // sideA Face
    addTri(out, tri(zero, three, four, sideANorm));
    addTri(out, tri(zero, four, seven, sideANorm));

    // sideB Face
    addTri(out, tri(one, two, five, sideBNorm));
    addTri(out, tri(one, two, six, sideBNorm));

    return true; 
}

bool setupVAOandVBO()
{
    std::vector<float> vertices;
    vertices.resize(48); // 6 (faces) * 2 (tris per face) * 4 (3 pos vertices + 1 norm per tri)
    makeCube(vertices);

    unsigned int VAO; // https://wikis.khronos.org/opengl/Vertex_Specification#Vertex_Array_Object
    {
        glGenVertexArrays(1, &VAO); // creates a VAO 'name'. Assigns it our passed in VAO address.
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(VAO);

    }

    unsigned int VBO; // https://wikis.khronos.org/opengl/Vertex_Specification#Vertex_Array_Object
    {
        glGenBuffers(1, &VBO); // creates VBO 'name'. Assigns it to our passed in VBO address.

        //-//////////////////////////////
        // glBindBuffer() - binds a buffer to GL target. If no matching buffer name is found, a buffer name is created.
        // param 1 - enum. target. binds buffer to this target.
        //      see doc for what target matches what buffer type.
        // param 2 - unsigned int. buffer name. *yes, the buffer name is an unsigned int*.
        // 
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBuffer.xhtml
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        //-//////////////////////////////
        // glVertexAttribPointer()
        // param 1 - unsigned int. position of vertex attribute we want to configure. 
        // param 2 - unsigned int. # of componeents per vertex attribute
        // param 3 - enum. Corresponds to the attribute's data type 
        // param 4 - bool. normalize data?
        // param 5 - size type. size of attribute or space between consecutive attributes
        // param 6 - (void*)unsigned int. offset to where the attribute's position is in the buffer.
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
        // ignoring the 4th component of gl_Position? Or are we not using that?
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
        glad_glEnableVertexAttribArray(0);

    }


    



    return true;
}

bool logShaderProgramInfo(unsigned int shaderProgramID)
{
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
    char logBuffer[2048];
    glGetProgramInfoLog(
        shaderProgramID, 
        2047,
        NULL,
        logBuffer);

    std::string message = "EMPTY LOG";
    if (sizeof(logBuffer) / sizeof(char) > 0 ) {
        message = logBuffer;
    }

    Logging::consoleLog(Logging::LogType::LOG, "\nPROGRAM INFO LOG AFTER LINKING:");
    Logging::consoleLog(Logging::LogType::LOG, message);
    Logging::consoleLog(Logging::LogType::LOG, "\nEND OF PROGRAM INFO LOG\n");

    return true;
}

#pragma endregion ================================

int main()
{
    Logging::consoleLog(Logging::LogType::LOG, \
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
        Logging::consoleLog(Logging::LogType::ASSERT,
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
    setupVAOandVBO();
    
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
     
    //-//////////////////
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

    Logging::consoleLog(Logging::LogType::LOG,
        (DASH_LINE + "\nENDING PROGRAM\n").c_str());
    Logging::closeLogFileIfOpen();

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
    Logging::consoleLog(Logging::LogType::LOG,
        "SETUP FOR WSL: " + setupResultString);

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
        Logging::consoleLog(Logging::LogType::LOG,
            ("INITIALIZED: TRUE"));
    }
    else
    {
        Logging::consoleLog(Logging::LogType::ASSERT,
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
        Logging::consoleLog(Logging::LogType::ASSERT,
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

#pragma endregion ================================

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
        platform = Platform::WSL;
    }
    
    if (platform == Platform::WSL && glfwPlatformSupported(GLFW_PLATFORM_X11)) 
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

#pragma endregion ================================

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
        // Closing and Close flag - https://www.glfw.org/docs/latest/window_guide.html#window_close
        glfwSetWindowShouldClose(window, true);
        return true;
    }

    return false;
}

#pragma endregion ================================

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

    return true;
}

#pragma region UTIL_FUNCTIONS

//-///////////////////////////////////////////////
//
static const bool IsNullPtr(void* pointer, std::string typeStr)
{
    if (pointer == NULL)
    {
        Logging::consoleLog(Logging::LogType::ERROR, 
            ("Pointer of type " + typeStr + "is null").c_str());
        return true;
    }

    return false;
}

#pragma endregion ================================