// Last LearnOpenGL page - https://learnopengl.com/Getting-started/Hello-Window
// GLSL Data Type page - https://wikis.khronos.org/opengl/Data_Type_(GLSL) 

//-/////////////////////////////////////////////////////////////////////
// BUILDING THIS PROGRAM FROM COMMAND LINE
// Use g++ since we're using Cpp.
// Add main.cpp and glad.c as source targets.
// -o param to create objects
// executable name
// link GLAD and GLFW libraries
// -lglfw -lGL -ldl     
//      -lglfw - ?? links GLFW ??
//      -lGL ?? links OpenGL ??
//      -ldl ?? what does this link ??

#pragma region HEADERS

// OPENGL-RELATED HEADERS
//-//////////////////////////
// glad
// - manages function pointers for OpenGL
#include "glad.h" // must include before GLFW
#include <GLFW/glfw3.h>

// CPP RELATED HEADERS
#include <iostream>

#pragma endregion

#pragma region PROTOYTPES

// CPP Prototypes
// Is this a CPP prototype??? Cuz the naming is C-style??
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

#pragma endregion

#pragma region FIELDS

const char* DASH_LINE = "--------------------------";

#pragma endregion

int main()
{
    std::cout << DASH_LINE << "\nSTARTING PROGRAM\n " << DASH_LINE << std::endl;

    // Initialize GLFW
    if (glfwInit() == false)
    {
        return -1;
    } 

    //-////////////////////////////////////////////////////////////////////
    // glfwWindowHint call - sets data for hints for next glfwCreateWindow call. 
    // - param 1 - select option from possible options prefixed with GLFW_.
    //      - full list of options found here: http://www.glfw.org/docs/latest/window.html#window_hints
    // - param 2 - integer. sets the value of our option. 
    // sets to OPENGL version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // CORE_PROFILE - the current set of features. 
    // COMPATITIBILITY_PROFILE would expose backwards-compatible features: https://community.khronos.org/t/opengl-core-profile-and-opengl-compat-profile/108643
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //-//////////////////////////////////////////////////////////////
    // glfwCreateWindow call. 
    //
    // param 1 - width (columns)
    // param 2 - height (rows)
    // param 3 - window name
    // param 4 - ?
    // param 5 - ?
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // make the created window the current context. 
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    // pass the OS-specific address of the OpenGL function pointers.
    // glfwGetProcAddress - defines the correct function based on which OS we're compiling for. 
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false)
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
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

    //-/////////////////////////////////////////////
    // RENDER LOOP
    //
    // glfwWindowShouldClose() call
    // - returns a flag. If true, do we close the window manually???? Or does glfw handle that??
    // ?? how is the flag set/determined ??
    while (glfwWindowShouldClose(window) == false)
    {
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

    std::cout << DASH_LINE << "\nENDING PROGRAM\n" << DASH_LINE << std::endl;
    return 0;
}

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


