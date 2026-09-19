#ifndef APPLICATION_H
#define APPLICATION_H

#include "ShaderLoader.h"
#include "Renderer.h"
#include "ShapeManager.h"
#include "TimeStepper.h"
#include "FileImporter.h"
// #include "FileManager.h"
#include "ErrorHandling.h"

#include <GLFW/glfw3.h>

class Application {
public:

    // Singleton access
    static Application& getInstance();  // Get singleton instance

    // Delete copy constructor and assignment operator to prevent copies
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Initialize the application (OpenGL, ImGui, etc.)
    void initialize(int argc, char** argv);

    // Start the application loop
    void run();

    // Getter for ShapeManager
    ShapeManager& getShapeManager();

    // Getter to retrieve the current TimeStepper
    static TimeStepper& getTimeStepper();

    // Setter to replace the current TimeStepper
    static void setTimeStepper(TimeStepper* newStepper);

    // Callback to resize viewport when window changes
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);


private:
    static Application* instance;  // Singleton instance
    
    Application();  // Constructor to set up defaults
    ~Application();  // Destructor for cleanup    

    static Renderer renderer;
    static ShapeManager shapeManager;
    static FileImporter fileImporter;
    static TimeStepper* timeStepper; 
//    static FileManager fileManager;
    
    GLFWwindow* window;  // Handle for GLFW window

    // Initialize OpenGL settings
    void initOpenGL();

    // Initialize ImGui settings
    void initImGui();

    // File manager utilities
    void saveScene();
    void loadScene();

};

#endif // APPLICATION_H
