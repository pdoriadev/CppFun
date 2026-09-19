#include "Globals.h"
#include "Application.h"
#include "Keyframe.h"
#include "ErrorHandling.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>                  // Core GLM types
#include <glm/gtc/matrix_transform.hpp> // Transformations (translate, rotate, scale)
#include <glm/gtc/type_ptr.hpp>         // To pass matrices to OpenGL shaders


// Unused?

#include <vector>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <array>
#include <algorithm>  // For std::find
#include "tinyfiledialogs.h"

#include "ColorPresets.h"
#include "Settings.h"
#include "Light.h"
#include "AnimationController.h"

// Static members
Renderer Application::renderer;
ShapeManager Application::shapeManager;
FileImporter Application::fileImporter;
// FileManager Application::fileManager;
TimeStepper* Application::timeStepper = nullptr;

// Initialize the static instance pointer to nullptr
Application* Application::instance = nullptr;


// Singleton access method
Application& Application::getInstance() {
    if (instance == nullptr) {
        instance = new Application();
    }
    return *instance;
}


Application::Application() {

    // Default ODE for TimeStepper
    timeStepper = new ForwardEuler();

    // Constructor: Initialize any defaults if needed

}

Application::~Application() {
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    shapeManager.getShapes().clear();  // Ensure all shapes are deleted before quitting

    // Clean up dynamically allocated resources
    delete timeStepper;
    
    // Cleanup glfw instance
    glfwDestroyWindow(window);
    glfwTerminate();
}


void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (height == 0) return; // Prevent division by zero

    // Update OpenGL viewport
    glViewport(0, 0, width, height);

    // Update the projection matrix to maintain the correct aspect ratio
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    // Apply new projection matrix to the shader
    GLuint shaderProgram = Application::renderer.getShaderProgram();
    glUseProgram(shaderProgram);
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Application::initialize(int argc, char** argv) {

    // Set GLFW error callback
    glfwSetErrorCallback(ErrorHandling::glfwErrorCallback);

    // Load persisted settings and apply them BEFORE glfwInit(). The windowing
    // platform is chosen by an init hint and Mesa reads its environment when
    // the context is created, so neither can be changed once GLFW is up - this
    // is why the Options dialog says a restart is required.
    SettingsIO::load(g_settings, SettingsIO::defaultPath());
    SettingsIO::applyStartupSettings(g_settings);

    // Mirror the per-user strings into the globals the rest of the code reads.
    if (!g_settings.customShapeName.empty()) customShapeName = g_settings.customShapeName;
    windowTitle = courseName + " - " + appName;
    if (!g_settings.studentName.empty()) windowTitle += " - " + g_settings.studentName;

    // Initialize GLFW
    if (!glfwInit()) {
        ErrorHandling::showError("Initialization Error", "Failed to initialize GLFW");
        exit(EXIT_FAILURE);
    }

    // Set GLFW window hints for OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // macOS REQUIRES this to hand out a Core Profile context at all. Apple's
    // driver only offers Core through the "forward compatible" path, so
    // without this hint glfwCreateWindow() below returns nullptr on every
    // Mac and the editor dies on launch with "Failed to create GLFW window"
    // and nothing else to go on. Assignments 0 and 1 both shipped without it
    // and both cost a support round-trip; this is the same bug.
    //
    // On Windows and Linux it is a harmless no-op: forward-compatible means
    // "the removed legacy functions are really gone", which is already true
    // of the Core profile requested on the line above.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create GLFW window
    window = glfwCreateWindow(800, 600, windowTitle.c_str(), nullptr, nullptr);
    if (!window) {
        ErrorHandling::showError("Window Error", "Failed to create GLFW window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Make the OpenGL context current
    glfwMakeContextCurrent(window);

    // Register framebuffer size callback for viewport resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        ErrorHandling::showError("GLAD Error", "Failed to initialize GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }


	// Enable OpenGL debugging through the ErrorHandling class
	if (GLAD_GL_VERSION_4_3 || GLAD_GL_KHR_debug) {
		ErrorHandling::enableOpenGLDebugging();
	} else {
		std::cerr << "OpenGL Debugging is not supported on this system." << std::endl;
	}


	// Test OpenGL context
	const GLubyte* openGLrenderer = glGetString(GL_RENDERER);
	const GLubyte* openGLversion = glGetString(GL_VERSION);
	if (!openGLrenderer || !openGLversion) {
		std::cerr << "Failed to create a valid OpenGL context." << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Renderer: " << openGLrenderer << "\nOpenGL Version: " << openGLversion << std::endl;

    // Set viewport size
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
	
    // Initialize OpenGL settings
    initOpenGL();

    // Initialize ImGui settings
    initImGui();

    // Every scene starts with one light, positioned where the old fixed light
    // was, so existing scenes look unchanged and the viewport is never black.
    // Placed inside the default view (camera orbits the origin at radius 5) so
    // the marker is on screen from the first frame. Roughly the same direction
    // as the old fixed light at (0, 6, 12), just nearer.
    shapeManager.addShape(new Light(1.5f, 3.0f, 4.0f,
                                    shapeManager.incrementShapeCounter()));

}


void Application::initOpenGL() {
    // Enable depth testing for 3D rendering
    glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);

    // Optional: Enable back-face culling for performance (if winding order is consistent)
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CCW);  // Counter-clockwise winding order for front faces

    // Set the background color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Set projection matrix
    float aspectRatio = 800.0f / 600.0f;  // Example aspect ratio
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    // Initialize shaders
    GLuint shaderProgram = ShaderLoader::loadShaderFromFile("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    if (shaderProgram == 0) {
        std::cerr << "Failed to load and compile shaders." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Pass projection matrix to the shader
    renderer.setShaderProgram(shaderProgram);
        
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Check for any OpenGL initialization errors
    ErrorHandling::checkOpenGLError("OpenGL Initialization");
}

void Application::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); 
	(void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Initialize ImGui with GLFW and OpenGL
    if (!ImGui_ImplOpenGL3_Init("#version 410")) {
        ErrorHandling::showError("ImGui Error", "Failed to initialize ImGui for OpenGL3");
        exit(EXIT_FAILURE);
    }
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        ErrorHandling::showError("ImGui Error", "Failed to initialize ImGui for GLFW");
        exit(EXIT_FAILURE);
    }

}

void Application::run() {

    // Keep track of time to compute deltaTime between frames
    float lastTime = glfwGetTime();

    // Main GLFW render loop
    while (!glfwWindowShouldClose(window)) {

        // Get current time and compute time difference from last frame
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        // Process user input and window events (keyboard, mouse, resize, etc.)
        glfwPollEvents();

        // Advance the simulation. The controller owns substeps, playback speed,
        // single-stepping, the simulation clock and the divergence guard.
        g_animation.stepSize = timeStepper->getStepSize();
        g_animation.update(shapeManager, timeStepper);

        // Keyframe playback. Independent of the particle systems: it is a
        // pure function of time, so it takes real seconds rather than the
        // integrator's step size.
        g_timeline.update(deltaTime, shapeManager);

        // Start a new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render the scene
        renderer.renderScene(shapeManager, timeStepper);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);

        // Check for OpenGL errors after each frame
        ErrorHandling::checkOpenGLError("Main Loop");
    }
}



// Getter implementation for ShapeManager
ShapeManager& Application::getShapeManager() {
    return shapeManager;
}

// Getter implementation for TimeStepper
TimeStepper& Application::getTimeStepper() {
    return *timeStepper;
}

// Setter implementation for TimeStepper
void Application::setTimeStepper(TimeStepper* newStepper) {
    if (timeStepper) {
        delete timeStepper; // Clean up the old instance to avoid memory leaks
    }
    timeStepper = newStepper; // Assign the new instance
}
/*
void Application::saveScene() {
    fileManager.saveScene(shapeManager);
}

void Application::loadScene() {
    fileManager.loadScene(shapeManager);
}

*/
