#include "Application.h"
#include "Renderer.h"
#include "Keyframe.h"
#include "TextureCache.h"
#include "FileImporter.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>                  // Core GLM types
#include <glm/gtc/matrix_transform.hpp> // Transformations (translate, rotate, scale)
#include <glm/gtc/type_ptr.hpp>         // To pass matrices to OpenGL shaders

#include "SceneLight.h"
#include "Settings.h"
#include "Light.h"
#include "SceneIO.h"
#include "AnimationController.h"
#include "Mobius.h"
#include "Torus.h"
#include "tinyfiledialogs.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>


namespace {

// Every numeric field in the panels uses Drag rather than Slider.
//
// This is not cosmetic: ImGui only turns a widget into a text box on
// double-click for the Drag variants (imgui_widgets.cpp, DragBehavior checks
// MouseClickedCount == 2; SliderBehavior only checks Ctrl+click). Transform
// already used DragFloat3, which is why typing worked there and nowhere else.
//
// AlwaysClamp matters because a typed value bypasses the drag range otherwise -
// without it you could type Transparency = 5 and get nonsense out of the tracer.
const ImGuiSliderFlags kTypeable = ImGuiSliderFlags_AlwaysClamp;

// What a shape's name would be if the user had never renamed it.
//
// serialType() is the class name and is right for every built-in shape. The
// importers are the exception: they overwrite shapeType with the source
// filename, and "ImportShape" would be a worse default than "bunny.obj", so
// recover the filename from the stored source path instead.
std::string defaultShapeName(const Shape* s) {
    if (!s) return "Shape";

    const std::string& src = s->getSourcePath();
    if (!src.empty()) {
        size_t slash = src.find_last_of("/\\");
        std::string file = (slash == std::string::npos) ? src : src.substr(slash + 1);
        size_t dot = file.find_last_of('.');
        if (dot != std::string::npos && dot > 0) file = file.substr(0, dot);
        if (!file.empty()) return file;
    }
    return s->serialType();
}

} // namespace


Renderer::Renderer()
    // Order matches the declaration order in Renderer.h - the compiler
    // initialises members in declaration order regardless of what is written
    // here, and -Wreorder fires when the two disagree.
    : showTracePanel(false),
      overlayPreview(false),
      showLightGizmos(true),
      showVRPanel(false),
      // Scene-file state. These were previously missing from the init list, so
      // savedSignature started as whatever was on the stack and the per-frame
      // comparison against it reported "unsaved changes" from the very first
      // frame: opening a scene in a freshly launched editor prompted to save
      // work that did not exist.
      sceneModified(false),
      savedSignature(0),
      signatureBaselined(false),
      confirmDiscardOpen(false),
      pendingSceneAction(0),
      leftPanelPushedStyles(false),
      showAnimationPanel(false),
      // Keyframe drag: nothing grabbed, nothing to undo.
      dragShapeId(-1), dragKeyIndex(-1),
      dragStartTime(0.0f), dragPressX(0.0f),
      dragArmed(false), dragActive(false),
      dragUndoShapeId(-1),
      showOptionsWindow(false),
      translateX(0.0f), translateY(0.0f), translateZ(0.0f),
      angleX(0.0f), angleY(0.0f), scaleFactor(1.0f),
      selectedIntegrator(IntegratorType::ForwardEuler) {
    // Camera constructs itself with the previous defaults
    // (theta = pi/2, phi = 0, radius = 5, target at origin).
}


// Getter for Shader Program
GLuint Renderer::getShaderProgram() const {
    return shaderProgram;
}


// Getter for Shader Program
void Renderer::setShaderProgram(GLuint shaderProg) {
    shaderProgram = shaderProg;
}


// Lighting setup
void Renderer::setupLighting(GLuint shaderProgram) {
    // Kept for compatibility; the real work needs the scene, so see
    // uploadLights() below.
    glUseProgram(shaderProgram);
}

// Uploads every enabled Light in the scene, plus the camera position that the
// specular term needs. viewPos was previously declared in the shader but never
// set, which left every highlight computed as though the eye sat at the origin.
void Renderer::uploadLights(GLuint shaderProgram, ShapeManager& shapeManager) {
    glUseProgram(shaderProgram);

    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1,
                 glm::value_ptr(camera.getPosition()));

    std::vector<Light*> lights = shapeManager.getLights();

    const int MAX_LIGHTS = 8;
    int count = 0;
    char name[64];

    for (size_t i = 0; i < lights.size() && count < MAX_LIGHTS; ++i) {
        Light* L = lights[i];
        if (!L || !L->isEnabled()) continue;

        glm::vec3 pos(L->getX(), L->getY(), L->getZ());

        std::sprintf(name, "lights[%d].type", count);
        glUniform1i(glGetUniformLocation(shaderProgram, name), L->getType());
        std::sprintf(name, "lights[%d].position", count);
        glUniform3fv(glGetUniformLocation(shaderProgram, name), 1, glm::value_ptr(pos));
        std::sprintf(name, "lights[%d].direction", count);
        glUniform3fv(glGetUniformLocation(shaderProgram, name), 1,
                     glm::value_ptr(L->getDirection()));

        glm::vec3 amb = L->effectiveAmbient();
        glm::vec3 dif = L->effectiveDiffuse();
        glm::vec3 spc = L->effectiveSpecular();
        std::sprintf(name, "lights[%d].ambient", count);
        glUniform3fv(glGetUniformLocation(shaderProgram, name), 1, glm::value_ptr(amb));
        std::sprintf(name, "lights[%d].diffuse", count);
        glUniform3fv(glGetUniformLocation(shaderProgram, name), 1, glm::value_ptr(dif));
        std::sprintf(name, "lights[%d].specular", count);
        glUniform3fv(glGetUniformLocation(shaderProgram, name), 1, glm::value_ptr(spc));

        std::sprintf(name, "lights[%d].constant", count);
        glUniform1f(glGetUniformLocation(shaderProgram, name), L->getConstantAttenuation());
        std::sprintf(name, "lights[%d].linear", count);
        glUniform1f(glGetUniformLocation(shaderProgram, name), L->getLinearAttenuation());
        std::sprintf(name, "lights[%d].quadratic", count);
        glUniform1f(glGetUniformLocation(shaderProgram, name), L->getQuadraticAttenuation());

        ++count;
    }

    // No Light objects: fall back to the legacy fixed light so the scene is
    // never rendered pitch black.
    if (count == 0) {
        glUniform1i(glGetUniformLocation(shaderProgram, "lights[0].type"), 0);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lights[0].position"), 1,
                     glm::value_ptr(g_sceneLight.position));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lights[0].ambient"), 1,
                     glm::value_ptr(g_sceneLight.ambient));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lights[0].diffuse"), 1,
                     glm::value_ptr(g_sceneLight.diffuse));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lights[0].specular"), 1,
                     glm::value_ptr(g_sceneLight.specular));
        glUniform1f(glGetUniformLocation(shaderProgram, "lights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "lights[0].linear"), 0.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "lights[0].quadratic"), 0.0f);
        count = 1;
    }

    glUniform1i(glGetUniformLocation(shaderProgram, "numLights"), count);
}


// Casts a ray through the cursor and selects whatever it hits.
bool Renderer::pickShape(ShapeManager& shapeManager, float mouseX, float mouseY,
                         int viewportWidth, int viewportHeight) {
    // Rebuild the scene each pick. Shapes move constantly in this editor, so a
    // cached hierarchy would go stale; a few thousand triangles rebuild fast
    // enough that correctness is worth more than the saved milliseconds.
    RayScene pickScene;
    pickScene.build(shapeManager);
    if (pickScene.empty()) return false;

    Ray ray = camera.generateRay(mouseX, mouseY, viewportWidth, viewportHeight);

    Hit hit;
    if (!pickScene.intersect(ray, 1e-4f, 1e30f, hit)) return false;
    if (!hit.shape) return false;

    shapeManager.setSelectedShape(const_cast<Shape*>(hit.shape));
    return true;
}


// Function to draw axis lines
void Renderer::drawAxis(GLuint shaderProgram) {

    // Use the shader program
    glUseProgram(shaderProgram);
    
    // Ensure model matrix is identity for the axis
    glm::mat4 model = glm::mat4(1.0f);
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Disable lighting for the axis
    GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 0); // Ensure lighting is OFF for the axis
    }

    // Vertex data for axis lines, including positions and colors
    float axisVertices[] = {
        // Positions        // Colors
        // X-axis (Red)
        -50.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // Origin
         50.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // +X

        // Y-axis (Green)
        0.0f, -50.0f, 0.0f,  0.0f, 1.0f, 0.0f, // Origin
        0.0f,  50.0f, 0.0f,  0.0f, 1.0f, 0.0f, // +Y

        // Z-axis (Blue)
        0.0f, 0.0f, -50.0f,  0.0f, 0.0f, 1.0f, // Origin
        0.0f, 0.0f,  50.0f,  0.0f, 0.0f, 1.0f  // +Z
    };

    GLuint axisVAO, axisVBO;
    // Generate and bind a Vertex Array Object (VAO)
    glGenVertexArrays(1, &axisVAO);
    glBindVertexArray(axisVAO);

    // Generate and bind a Vertex Buffer Object (VBO)
    glGenBuffers(1, &axisVBO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    // Configure the position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Configure the color attribute (location 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Draw the axis lines
    glBindVertexArray(axisVAO);
    glLineWidth(1.0f); // Choose a float value > 1.0f for thicker lines
    glDrawArrays(GL_LINES, 0, 6);

    // Re-enable lighting for shapes
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 1);
    }

    // Unbind and clean up
    glBindVertexArray(0);
    glDeleteBuffers(1, &axisVBO);
    glDeleteVertexArrays(1, &axisVAO);
    
}


// Render the scene and the shapes
void Renderer::renderScene(ShapeManager& shapeManager, TimeStepper* timeStepper) {

    // Drop keyframe tracks whose shape has been deleted.
    //
    // Here rather than in the delete handler on purpose. A track outliving its
    // shape is unreachable -- every button in the keyframe section acts on the
    // SELECTED shape and a deleted one cannot be selected -- so the only way
    // out was to edit the scene file by hand. Doing it per frame means no
    // deletion path can miss it, including ones that do not exist yet, which is
    // the same reasoning sceneSignature() uses for dirty tracking.
    if (g_timeline.pruneOrphanTracks(shapeManager) > 0) {
        sceneModified = true;

        // A pruned track can invalidate a drag in progress: the grabbed key may
        // have just been erased along with its track.
        dragArmed = dragActive = false;
        dragShapeId = -1;
        dragUndoShapeId = -1;
    }

    // Get the latest window size
    int width, height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

    // Ensure viewport matches window size
    glViewport(0, 0, width, height);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start the ImGui frame
    ImGuiIO& io = ImGui::GetIO();

    // Main Menu Bar
    if (ImGui::BeginMainMenuBar()) {
        drawFileMenu(shapeManager);

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Insert")) {
            if (ImGui::MenuItem("Cube")) {
				shapeManager.addShape(new Cube(0.0f, 0.0f, 0.0f, 1.0f, 2, shapeManager.incrementShapeCounter()));
				shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
            }
            if (ImGui::MenuItem("Sphere")) {
				shapeManager.addShape(new Sphere(0.0f, 0.0f, 0.0f, 1.0f, 3, shapeManager.incrementShapeCounter()));
				shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
            }
            if (ImGui::MenuItem("Pyramid")) {
				shapeManager.addShape(new Pyramid(0.0f, 0.0f, 0.0f, 1.0f, 5, shapeManager.incrementShapeCounter()));
				shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
            }
            if (ImGui::MenuItem("Teapot")) {
				shapeManager.addShape(new Teapot(0.0f, 0.0f, 0.0f, 1.0f, 4, shapeManager.incrementShapeCounter()));
				shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
            }
            if (ImGui::MenuItem("Mobius Strip")) {
                shapeManager.addShape(new Mobius(0.0f, 0.0f, 0.0f, 1.0f, 3,
                                                 shapeManager.incrementShapeCounter()));
                shapeManager.setSelectedShapeByLastAdded();
            }
            if (ImGui::MenuItem("Torus")) {
                shapeManager.addShape(new Torus(0.0f, 0.0f, 0.0f, 1.0f, 3,
                                                shapeManager.incrementShapeCounter()));
                shapeManager.setSelectedShapeByLastAdded();
            }
            if (ImGui::MenuItem("Light")) {
                shapeManager.addShape(new Light(0.0f, 6.0f, 12.0f,
                                                shapeManager.incrementShapeCounter()));
                shapeManager.setSelectedShapeByLastAdded();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Icosahedron")) {
				shapeManager.addShape(new Icosahedron(0.0f, 0.0f, 0.0f, 1.0f, 6, shapeManager.incrementShapeCounter()));
				shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
            }
            if (ImGui::MenuItem(customShapeName.c_str())) {
				shapeManager.addShape(new Custom(0.0f, 0.0f, 0.0f, 1.0f, 11, shapeManager.incrementShapeCounter()));
				shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
            }
			
			ImGui::Separator();

            if (ImGui::MenuItem("Simple System")) {
				shapeManager.addShape(new SimpleSystem(0.0f, 0.0f, 0.0f, 1.0f, 6, shapeManager.incrementShapeCounter())); 			
				shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
            }

            if (ImGui::MenuItem("Pendulum System")) {
				shapeManager.addShape(new SimplePendulum(0.0f, 0.0f, 0.0f, 1.0f, 1, shapeManager.incrementShapeCounter(),
				2.0f,              // Length of pendulum
				1.0f               // Mass of bob
				)); 			
				shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
            }

            if (ImGui::MenuItem("Chain System")) {
				shapeManager.addShape(new SimpleChain(0.0f, 0.0f, 0.0f, 1.0f, 1, shapeManager.incrementShapeCounter(),
				2.0f,              // Length of chain
				1.0f               // Mass of bobs
				)); 			
				shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
            }

            if (ImGui::MenuItem("Cloth System")) {
				shapeManager.addShape(new SimpleCloth(0.0f, 0.0f, 0.0f, 1.0f, 6, shapeManager.incrementShapeCounter(),
				2.0f,               // Length of chain
				0.1f,               // Mass of bobs
				15					// Dimension of cloth
				)); 			
				shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
            }

	    ImGui::Separator();

	    if (ImGui::BeginMenu("Import"))
	    {

	        if (ImGui::MenuItem("Import Shape")) {
	            FileImporter fileImporter;  // This creates an instance of the FileImporter class
	            if (fileImporter.importObjFile(shapeManager)) {  // Function to import .obj files
	                shapeManager.setSelectedShapeByLastAdded();
		    }
	        }

	        if (ImGui::MenuItem("Import Curve")) {
	            FileImporter fileImporter;  // This creates an instance of the FileImporter class
	            if (fileImporter.importSwpFile(shapeManager)) {  // Function to import .obj files
		        shapeManager.setSelectedShapeByLastAdded();
		    }
	        }

	        if (ImGui::MenuItem("Import Character")) {
	            FileImporter fileImporter;  // This creates an instance of the FileImporter class
	            if (fileImporter.importCharacterFile(shapeManager)) {  // Function to import .obj files
		        shapeManager.setSelectedShapeByLastAdded();
		    }
	        }

	        ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Render")) {
            ImGui::MenuItem("Ray Tracer", NULL, &showTracePanel);
            ImGui::MenuItem("VR / Head Tracking", NULL, &showVRPanel);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {

            // Camera view presets
            if (ImGui::MenuItem("View from +X")) {
                camera.setView(0.0f, 0.0f, 5.0f);
            }
            if (ImGui::MenuItem("View from -X")) {
                camera.setView(glm::pi<float>(), 0.0f, 5.0f);
            }
            if (ImGui::MenuItem("View from +Y")) {
                camera.setView(glm::pi<float>() / 2.0f, glm::half_pi<float>(), 5.0f);
            }
            if (ImGui::MenuItem("View from -Y")) {
                camera.setView(glm::pi<float>() / 2.0f, -glm::half_pi<float>(), 5.0f);
            }
            if (ImGui::MenuItem("View from +Z")) {
                camera.setView(glm::pi<float>() / 2.0f, 0.0f, 5.0f);
            }
            if (ImGui::MenuItem("View from -Z")) {
                camera.setView(glm::pi<float>() * 1.5f, 0.0f, 5.0f);
            }

            ImGui::Separator();

            // Reset to default camera view
            if (ImGui::MenuItem("Reset to Default Camera View")) {
                camera.setView(glm::pi<float>() / 2.0f, 0.0f, 5.0f);
            }

            ImGui::Separator();

            ImGui::MenuItem("Show Light Markers", NULL, &showLightGizmos);

            ImGui::Separator();

            // Toggle axis visibility with updated label
            std::string axisToggleText = showAxis ? "Toggle Axis Off" : "Toggle Axis On";
            if (ImGui::MenuItem(axisToggleText.c_str(),"T")) {
                showAxis = !showAxis;
            }

            ImGui::EndMenu();
        }
        drawAnimationMenu(shapeManager);

        // Frame rate readout, right-aligned in the menu bar.
        //
        // The editor previously reported nothing about its own speed, which
        // made "is this slow because of my scene, my driver, or my code?"
        // impossible to answer without external tools. io.Framerate is ImGui's
        // own rolling average over the last 60 frames, so it does not jitter
        // the way an instantaneous 1/deltaTime does.
        {
            char perf[64];
            std::snprintf(perf, sizeof(perf), "%.1f FPS  (%.2f ms)",
                          io.Framerate, 1000.0f / (io.Framerate > 0.0f ? io.Framerate : 1.0f));
            float w = ImGui::CalcTextSize(perf).x;
            ImGui::SameLine(ImGui::GetWindowWidth() - w - 16.0f);
            ImGui::TextDisabled("%s", perf);
        }

        ImGui::EndMainMenuBar();
    }
    
    // Check if the Esc key was pressed using ImGui
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        exit(0);  // Exit the program if the Esc key is pressed
    }    

    // Check for keyboard shortcut 'T' (toggles axis visibility)
    if (!ImGui::IsAnyItemActive() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
        if (ImGui::IsKeyPressed(ImGuiKey_T)) {
            showAxis = !showAxis;
        }
    }

    // Right-side ImGui panel for shape selection and properties
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 280, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(280, io.DisplaySize.y - 20), ImGuiCond_Always);
    ImGui::Begin("Shape Selector and Properties", NULL, ImGuiWindowFlags_NoResize);

    // Shape Selector Panel
    ImGui::Text("Shape Selector");
    for (size_t i = 0; i < shapeManager.getShapes().size(); i++) {
        Shape* shape = shapeManager.getShapeAt(i);
        if (!shape) continue;

        // snprintf, and 128 bytes rather than 64. Names became user-editable
        // with the rename field, and the old sprintf into char[64] would run
        // off the end of the buffer for any name past about 50 characters -
        // "[12] " plus a 63-character name plus " (ID: 12)" does not fit.
        char buffer[128];
        std::snprintf(buffer, sizeof(buffer), "[%d] %s (ID: %d)",
                      static_cast<int>(i) + 1,
                      shape->getShapeType().c_str(), shape->getId());
        
        if (ImGui::Selectable(buffer, shape == shapeManager.getSelectedShape())) {
			shapeManager.setSelectedShapeById(shape->getId()); 
        }
		
    }

    ImGui::Separator();

    // Shape Properties Panel
    if (shapeManager.getSelectedShape()) {
        ImGui::Text("Selected Shape ID: %d", shapeManager.getSelectedShape()->getId());

        // Rename the instance.
        //
        // shapeType starts as the class name ("Cube", "Sphere") or, for the
        // importers, the source filename. Nothing branches on the string - it
        // is purely what the shape list and the scene file display - so it is
        // safe to let the user overwrite it, and a scene with six cubes in it
        // is unnavigable otherwise.
        {
            Shape* sel = shapeManager.getSelectedShape();

            static char nameBuf[64] = "";
            static const Shape* nameBufOwner = 0;

            // Keyed on the object, not its id: ids restart at 1 in every scene,
            // so keying on the id would leave a stale name in the box after
            // loading a different file.
            //
            // Refill only when the selection changes. Copying every frame would
            // overwrite the buffer underneath the user while they are typing.
            if (nameBufOwner != sel) {
                std::snprintf(nameBuf, sizeof(nameBuf), "%s", sel->getShapeType().c_str());
                nameBufOwner = sel;
            }

            if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
                // Ignore an empty field rather than committing a blank name;
                // the user is usually mid-way through clearing it to retype.
                if (nameBuf[0] != '\0') sel->setShapeType(nameBuf);
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset##name")) {
                sel->setShapeType(defaultShapeName(sel));
                std::snprintf(nameBuf, sizeof(nameBuf), "%s", sel->getShapeType().c_str());
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Back to the default name for this shape type");
            }
        }

        // Lights carry their own colour, so the preset picker is meaningless
        // for them and would silently do nothing.
        const bool selectionIsLight =
            (dynamic_cast<Light*>(shapeManager.getSelectedShape()) != 0);

        // PushID per section: a CollapsingHeader does not open an ID scope, so
        // without this a header and any widget inside it that share a label
        // resolve to the same ImGui ID and trigger a conflict assert.
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID("transformSection");
	// Position
        float pos[3] = { shapeManager.getSelectedShape()->getX(), shapeManager.getSelectedShape()->getY(), shapeManager.getSelectedShape()->getZ() };
        if (ImGui::DragFloat3("Position", pos, 0.1f)) {
            shapeManager.getSelectedShape()->setPosition(pos[0], pos[1], pos[2]);
        }

	// Scaling options
	bool useUniformScaling = shapeManager.getSelectedShape()->isUsingUniformScaling();
	if (ImGui::Checkbox("Uniform Scaling", &useUniformScaling)) {
		shapeManager.getSelectedShape()->useUniformScaling(useUniformScaling);
	}

	if (useUniformScaling) {
		float scale = shapeManager.getSelectedShape()->getScale();
		if (ImGui::DragFloat("Uniform Scale", &scale, 0.1f, 0.1f, 10.0f)) {
			shapeManager.getSelectedShape()->setScale(scale);
		}
	} else {
		glm::vec3 nonUniformScale = shapeManager.getSelectedShape()->getNonUniformScale();
		float scale[3] = {nonUniformScale.x, nonUniformScale.y, nonUniformScale.z};
		if (ImGui::DragFloat3("Scale (X, Y, Z)", scale, 0.1f, 0.1f, 10.0f)) {
			shapeManager.getSelectedShape()->setScale(scale[0], scale[1], scale[2]);
		}
	}

	// Rotation
        float rotation[3] = { shapeManager.getSelectedShape()->getAngleX(), shapeManager.getSelectedShape()->getAngleY(), shapeManager.getSelectedShape()->getAngleZ() };
        if (ImGui::DragFloat3("Rotation", rotation, 1.0f, -360.0f, 720.0f)) {
            shapeManager.getSelectedShape()->setRotation(rotation[0], rotation[1], rotation[2]);
        }

            ImGui::PopID();
        }

        if (!selectionIsLight &&
            ImGui::CollapsingHeader("Color", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID("colorSection");
        // Color Selection
        static int selectedColorIdx = 0;
        static float customColor[3] = {0.0f, 0.0f, 0.0f};  // Initialize for custom color picker

        if (shapeManager.getSelectedShape()) {
            selectedColorIdx = shapeManager.getSelectedShape()->getColorIndex();  // Sync with shape's current color

            // If it's a custom color, update customColor array with shape's current custom color
            if (selectedColorIdx == 31) {
                const float* currentCustomColor = shapeManager.getSelectedShape()->getCustomColor();
                customColor[0] = currentCustomColor[0];
                customColor[1] = currentCustomColor[1];
                customColor[2] = currentCustomColor[2];
            }
        }


        if (ImGui::BeginCombo("Preset", colorPresets[selectedColorIdx].name)) {
            for (int i = 0; i < 32; i++) {
                ImGui::PushID(i);
                ImGui::ColorButton("##color", ImVec4(colorPresets[i].color[0], colorPresets[i].color[1], colorPresets[i].color[2], 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs, ImVec2(20, 20));
                ImGui::SameLine();
                if (ImGui::Selectable(colorPresets[i].name, selectedColorIdx == i)) {
                    selectedColorIdx = i;
                    if (i != 31) {  // Not "Custom"
                        shapeManager.getSelectedShape()->setColor(i);
                    } else {
                        // If "Custom" is selected, preserve the current custom color
                        shapeManager.getSelectedShape()->setColor(i);
                        shapeManager.getSelectedShape()->setCustomColor(customColor[0], customColor[1], customColor[2]);
                    }
                }
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }

        // Custom Color Picker if "Custom" is selected
        if (selectedColorIdx == 31) {
            if (ImGui::ColorEdit3("Custom Color", customColor)) {
                // Update the custom color preset and shape color dynamically
                shapeManager.getSelectedShape()->setColor(31); // Use custom color
                shapeManager.getSelectedShape()->setCustomColor(customColor[0], customColor[1], customColor[2]);
            }
        }


            ImGui::PopID();
        }

        ImGui::Separator();

        // Material for ordinary shapes, light attributes for lights.
        drawSelectedObjectProperties(shapeManager);

        ImGui::Separator();

		// For ImportCurves only

		if (ImportCurve* importCurve = dynamic_cast<ImportCurve*>(shapeManager.getSelectedShape())) {
			// Toggle for showing control points
			bool showControlPoints = importCurve->isControlPointsVisible();
			if (ImGui::Checkbox("Show Control Points", &showControlPoints)) {
				importCurve->setControlPointsVisible(showControlPoints);
			}

			// Combo box for curve visibility mode
			const char* curveOptions[] = { "Off", "On", "On with Normals/Tangents/Binormals" };
			int curveVisibilityMode = importCurve->getCurveVisibilityMode();
			ImGui::Text("Curve Visibility");  // This adds the label above the dropdown
			ImGui::SetNextItemWidth(200);  // Adjust the dropdown width as needed			
			if (ImGui::Combo("##curveVisibility", &curveVisibilityMode, curveOptions, IM_ARRAYSIZE(curveOptions))) {
				importCurve->setCurveVisibilityMode(curveVisibilityMode);
			}
			
			// Combo box for surface visibility mode
			const char* surfaceOptions[] = { "Off", "Wireframe with Normals", "Shaded" };
			int surfaceVisibilityMode = importCurve->getSurfaceVisibilityMode();
			ImGui::Text("Surface Visibility");  // This adds the label above the dropdown
			ImGui::SetNextItemWidth(200);  // Adjust the dropdown width as needed			
			if (ImGui::Combo("##surfaceVisibility", &surfaceVisibilityMode, surfaceOptions, IM_ARRAYSIZE(surfaceOptions))) {
				importCurve->setSurfaceVisibilityMode(surfaceVisibilityMode);
			}
						
		}

		// For ImportCharacter only


        ImGui::Separator();
        if (ImportCharacter* importCharacter = dynamic_cast<ImportCharacter*>(shapeManager.getSelectedShape())) {

			ImGui::Separator();
			ImGui::Text("Model Display Mode");

			// Change display mode between mesh and skeletal
			const char* modelModes[] = { "Skeletal Model", "Mesh Model" };
			int currentMode = importCharacter->getDisplayMode();  // Assume this function exists in ImportCharacter

			if (ImGui::Combo("Display Mode", &currentMode, modelModes, IM_ARRAYSIZE(modelModes))) {
				importCharacter->setDisplayMode(static_cast<ImportCharacter::DisplayMode>(currentMode));  // Cast to DisplayMode
			}

            ImGui::Separator();
            ImGui::Text("Joint Rotations (x, y, z)");

			// Get references to vertices and bindVertices
			std::vector<glm::vec3>& currentVertices = importCharacter->getVertices();  // Direct access to vertices from Shape
			const std::vector<glm::vec3>& bindVertices = importCharacter->getBindVertices();
			const std::vector<std::vector<float>>& attachments = importCharacter->getAttachments();



			// Call updateMesh on the skeletal model to update current vertices
			// importCharacter->updateMeshVertices();

			// ImGui control to apply joint transformations and trigger the mesh update
			for (size_t i = 0; i < importCharacter->getSkeletalModel().getJoints().size(); ++i) {

				// Retrieve stored rotation as a mutable array
				glm::vec3 jointRotationVec = importCharacter->getSkeletalModel().getJoints()[i]->getRotation();
				float jointRotation[3] = { jointRotationVec.x, jointRotationVec.y, jointRotationVec.z };

				// ImGui sliders for joint rotation control with 1 degree per unit
				char label[128];
				sprintf(label, "%s", jointName[i].c_str()); // Construct the label using the joint name
				if (ImGui::DragFloat3(label, jointRotation, 1.0f, -360.0f, 720.0f)) {

					jointRotation[0] = (jointRotation[0] < 0) ? 360.0f + fmod(jointRotation[0], 360.0f) : fmod(jointRotation[0], 360.0f);
					jointRotation[1] = (jointRotation[1] < 0) ? 360.0f + fmod(jointRotation[1], 360.0f) : fmod(jointRotation[1], 360.0f);
					jointRotation[2] = (jointRotation[2] < 0) ? 360.0f + fmod(jointRotation[2], 360.0f) : fmod(jointRotation[2], 360.0f);

					// Set joint transform with updated rotations
					importCharacter->getSkeletalModel().setJointTransform(i, jointRotation[0], jointRotation[1], jointRotation[2]);
					importCharacter->getSkeletalModel().updateCurrentJointToWorldTransforms();

				}
			}
        }


		// For SimplePendulum only

		if (PendulumSystem* simplePendulum = dynamic_cast<PendulumSystem*>(shapeManager.getSelectedShape())) {
			ImGui::Text("Pendulum System Properties");

			// Mass adjustment slider
			float mass = simplePendulum->getMass();
			if (ImGui::DragFloat("Mass", &mass, 0.02f, 0.1f, 10.0f, "%.2f", kTypeable)) {
				simplePendulum->setMass(mass);
			}
		}



		// For SimpleCloth only

		if (SimpleCloth* simpleCloth = dynamic_cast<SimpleCloth*>(shapeManager.getSelectedShape())) {

			ImGui::Text("Simple Cloth Properties");

			// Toggle for wireframe mode
			bool isWireframeOn = simpleCloth->getWireframe(); // Get current state
			if (ImGui::Checkbox("Enable Wireframe", &isWireframeOn)) {
				if (isWireframeOn) {
					simpleCloth->enableWireframe(); // Turn on wireframe
				} else {
					simpleCloth->disableWireframe(); // Turn off wireframe
				}
			}

			// Toggle for particles visibility
			bool isParticlesOn = simpleCloth->getParticles(); // Get current state
			if (ImGui::Checkbox("Show Particles", &isParticlesOn)) {
				if (isParticlesOn) simpleCloth->enableParticles();
				else simpleCloth->disableParticles();
			}

			// Toggle for structural springs visibility
			bool isStructSpringsOn = simpleCloth->getStructSprings(); // Get current state
			if (ImGui::Checkbox("Show Structural Springs", &isStructSpringsOn)) {
				if (isStructSpringsOn) simpleCloth->enableStructSprings();
				else simpleCloth->disableStructSprings();
			}

			// Toggle for movement
			bool isMovementOn = simpleCloth->getMovement(); // Get current state
			if (ImGui::Checkbox("Enable Movement", &isMovementOn)) {
				if (isMovementOn) {
					simpleCloth->enableMovement(); // Turn on movement
				} else {
					simpleCloth->disableMovement(); // Turn off movement
				}
			}


			// Toggle for wind
			bool isWindOn = simpleCloth->getWind();
			if (ImGui::Checkbox("Enable Wind", &isWindOn)) {
				// Update the wind state based on the checkbox's value
				if (isWindOn) {
					simpleCloth->enableWind();
				} else {
					simpleCloth->disableWind();
				}
			}

			// Wind Direction

			ImGui::PushItemWidth(150); // Set width to 150 pixels (adjust as needed)

			// Wind Direction Dropdown (with diagonal directions)
                        const char* directions[] = {
                            "Right", "Left", "Forward", "Backward",
                            "Forward-Right", "Forward-Left", "Backward-Right", "Backward-Left"
                        };
                        static int currentDirection = 0; // Default: +X

                        if (ImGui::Combo("Wind Direction", &currentDirection, directions, IM_ARRAYSIZE(directions))) {
                            switch (currentDirection) {
                                case 0: simpleCloth->setWindDirection(glm::vec3(1.0f, 0.0f, 0.0f)); break;  // +X
                                case 1: simpleCloth->setWindDirection(glm::vec3(-1.0f, 0.0f, 0.0f)); break; // -X
                                case 2: simpleCloth->setWindDirection(glm::vec3(0.0f, 0.0f, 1.0f)); break;  // +Z
                                case 3: simpleCloth->setWindDirection(glm::vec3(0.0f, 0.0f, -1.0f)); break; // -Z
                                case 4: simpleCloth->setWindDirection(glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f))); break;   // +X +Z
                                case 5: simpleCloth->setWindDirection(glm::normalize(glm::vec3(-1.0f, 0.0f, 1.0f))); break;  // -X +Z
                                case 6: simpleCloth->setWindDirection(glm::normalize(glm::vec3(1.0f, 0.0f, -1.0f))); break;  // +X -Z
                                case 7: simpleCloth->setWindDirection(glm::normalize(glm::vec3(-1.0f, 0.0f, -1.0f))); break; // -X -Z
                            }
                        }


			// Wind Intensity
			float intensity = simpleCloth->getWindIntensity();
			if (ImGui::DragFloat("Wind Intensity", &intensity, 0.02f, 0.5f, 10.0f, "%.1f", kTypeable)) {
				simpleCloth->setWindIntensity(intensity);
			}

			ImGui::PopItemWidth(); // Restore the default width

		}

        // Add the Delete button
        ImGui::Separator();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
        if (ImGui::Button("Delete", ImVec2(120, 30))) {
            // Delete the selected shape
			shapeManager.deleteShape(shapeManager.getSelectedShape());
        }
        ImGui::PopStyleVar();  // Restore the style

        ImGui::SameLine();  // Place the Reset button on the same line as the Delete button

        // Add the Reset button
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
        if (ImGui::Button("Reset", ImVec2(120, 30))) {

            // Reset the selected shape to default settings
            shapeManager.getSelectedShape()->resetToDefault();

			ImportCharacter* importChar = dynamic_cast<ImportCharacter*>(shapeManager.getSelectedShape());

			if (importChar) {
				importChar->resetPose(); // Call the resetPose method if it's an ImportCharacter
			}
        }
        ImGui::PopStyleVar();  // Restore the style

    }

    ImGui::End();

    // Recompute the dirty state from scene content every frame.
    //
    // The first frame that sees a populated scene establishes the baseline
    // rather than comparing against one: the startup scene is assembled by
    // Application after this Renderer is constructed, so until it exists there
    // is nothing meaningful to compare to.
    if (!signatureBaselined) {
        if (!shapeManager.getShapes().empty()) {
            savedSignature     = sceneSignature(shapeManager);
            sceneModified      = false;
            signatureBaselined = true;
        }
    } else {
        sceneModified = (sceneSignature(shapeManager) != savedSignature);
    }

    // Left dock starts just below the main menu bar.
    leftPanelY = ImGui::GetFrameHeight();

    drawSceneModals(shapeManager);

    // Ray tracing controls
    drawRayTracePanel(shapeManager);
    drawAnimationPanel(shapeManager);
    drawOptionsWindow();
    drawVRPanel();

    if (g_settings.autoEnterVR && vrSystem.sourceIsDevice() && !vrSystem.active()) {
        vrSystem.enter();
    }
    vrSystem.update(io.DeltaTime, camera);

    // Handle mouse wheel zoom (scrolling in/out)
    if (!ImGui::IsAnyItemActive() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
        if (io.MouseWheel != 0 && !vrSystem.active()) {
            float zoomSpeed = 0.5f;
            camera.zoom(io.MouseWheel * zoomSpeed);
        }
    }

    // Ensure no UI elements are being interacted with before handling inputs
    if (!ImGui::IsAnyItemActive() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {

        // Left Click + Drag → Pan Camera
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            float panSpeed = 0.01f;
            camera.pan(io.MouseDelta.x * panSpeed, io.MouseDelta.y * panSpeed);
        }

        // Right Click + Drag → Rotate Camera
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right) && !vrSystem.active()) {
            float rotationSpeed = 0.005f;
            camera.orbit(-io.MouseDelta.x * rotationSpeed, -io.MouseDelta.y * rotationSpeed);
        }

        // Left click WITHOUT a drag → ray-cast pick.
        // Left-drag already pans, so we only treat a release as a selection
        // when the cursor barely moved since the press.
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
            !(vrSystem.active() && vrSystem.settings.sideBySide)) {
            ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f);
            if (std::fabs(drag.x) < 3.0f && std::fabs(drag.y) < 3.0f) {
                // Cursor is in window coordinates; rays are generated in
                // framebuffer pixels, which differ on high-DPI displays.
                int winW = 0, winH = 0;
                glfwGetWindowSize(glfwGetCurrentContext(), &winW, &winH);
                float scaleX = (winW > 0) ? static_cast<float>(width)  / winW : 1.0f;
                float scaleY = (winH > 0) ? static_cast<float>(height) / winH : 1.0f;

                pickShape(shapeManager,
                          io.MousePos.x * scaleX,
                          io.MousePos.y * scaleY,
                          width, height);
            }
        }
    }


    // Get the shader program and activate it
    GLuint shaderProgram = getShaderProgram();
    glUseProgram(shaderProgram);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const bool stereo = vrSystem.active() && vrSystem.settings.sideBySide;
    const int eyeCount = stereo ? 2 : 1;
    for (int eyeIndex = 0; eyeIndex < eyeCount; ++eyeIndex) {
        const int viewportWidth = stereo ? width / 2 : width;
        glViewport(stereo ? eyeIndex * viewportWidth : 0, 0, viewportWidth, height);

        if (stereo) {
            camera.setViewOverride(vrSystem.viewForEye(
                camera, eyeIndex == 0 ? VR_EYE_LEFT : VR_EYE_RIGHT));
        }

        const glm::mat4 viewMatrix = camera.getViewMatrix();
        const float aspectRatio = static_cast<float>(viewportWidth) /
                                  static_cast<float>(height > 0 ? height : 1);
        const glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1,
                           GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                           GL_FALSE, glm::value_ptr(projection));

        uploadLights(shaderProgram, shapeManager);
        if (showAxis) drawAxis(shaderProgram);

        for (Shape* shape : shapeManager.getShapes()) {
            if (!showLightGizmos && dynamic_cast<Light*>(shape)) continue;
            shape->applyTransform(shaderProgram);
            shape->applyMaterial(shaderProgram);
            shape->draw(shaderProgram);
        }
    }
    glViewport(0, 0, width, height);

    // Advance the ray tracer a slice at a time so the UI stays responsive and
    // the preview fills in progressively.
    if (rayTracer.isRendering()) {
        rayTracer.step(0.020);   // ~20 ms per frame
    }

    // Optionally cover the 3D view with the traced image
    drawPreviewOverlay();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  
    // Disable 
    glUseProgram(0);
}

void Renderer::drawVRPanel() {
    if (!showVRPanel) return;
    if (!beginLeftPanel("VR / Head Tracking", &showVRPanel)) {
        endLeftPanel();
        return;
    }

    const char* sources[] = { "Simulated pose", "OpenXR headset" };
    ImGui::Combo("Source", &vrSystem.settings.source, sources, 2);
    ImGui::TextWrapped("%s", vrSystem.status().c_str());

    std::string unavailable;
    const bool canEnter = vrSystem.available(unavailable);
    if (vrSystem.active()) {
        if (ImGui::Button("Exit VR")) vrSystem.exit();
    } else {
        if (!canEnter) ImGui::BeginDisabled();
        if (ImGui::Button("Enter VR")) vrSystem.enter();
        if (!canEnter) ImGui::EndDisabled();
        if (!canEnter) ImGui::TextWrapped("Unavailable: %s", unavailable.c_str());
    }

    ImGui::Separator();
    ImGui::Checkbox("Both eyes, side by side", &vrSystem.settings.sideBySide);
    ImGui::RadioButton("Left eye", &vrSystem.settings.eye, VR_EYE_LEFT);
    ImGui::SameLine();
    ImGui::RadioButton("Right eye", &vrSystem.settings.eye, VR_EYE_RIGHT);
    ImGui::DragFloat("World units / metre", &vrSystem.settings.worldUnitsPerMetre,
                     0.05f, 0.01f, 100.0f, "%.2f", kTypeable);

    if (vrSystem.settings.source == VRSystem::Source_Simulated) {
        ImGui::SeparatorText("Simulator");
        ImGui::RadioButton("Canned path", &vrSystem.settings.simMode, VRSystem::Sim_Track);
        ImGui::SameLine();
        ImGui::RadioButton("Manual", &vrSystem.settings.simMode, VRSystem::Sim_Manual);
        if (vrSystem.settings.simMode == VRSystem::Sim_Track) {
            ImGui::Checkbox("Play", &vrSystem.settings.simPlaying);
            ImGui::SliderFloat("Time", &vrSystem.settings.simTime, 0.0f,
                               VRSystem::trackDuration(), "%.2f s");
        } else {
            ImGui::DragFloat("Yaw", &vrSystem.settings.simYawDeg, 0.5f, -180.0f, 180.0f);
            ImGui::DragFloat("Pitch", &vrSystem.settings.simPitchDeg, 0.5f, -90.0f, 90.0f);
            ImGui::DragFloat("Roll", &vrSystem.settings.simRollDeg, 0.5f, -180.0f, 180.0f);
            ImGui::DragFloat3("Position (m)", &vrSystem.settings.simX, 0.01f);
        }
        ImGui::DragFloat("IPD (mm)", &vrSystem.settings.simIpdMm, 0.1f,
                         45.0f, 85.0f, "%.1f", kTypeable);
    }

    ImGui::Checkbox("Auto-enter with real device", &g_settings.autoEnterVR);
    const HeadPose& pose = vrSystem.pose();
    const glm::vec3 p = glm::vec3(pose.headToPlaySpace[3]);
    ImGui::Text("Tracking: %s", pose.valid ? "valid" : "inactive");
    ImGui::Text("Head: %.3f  %.3f  %.3f m", p.x, p.y, p.z);
    endLeftPanel();
}



// ---------------------------------------------------------------------------
// Ray tracing UI
// ---------------------------------------------------------------------------

void Renderer::drawRayTracePanel(ShapeManager& shapeManager) {

    if (!showTracePanel) return;

    beginLeftPanel("Ray Tracer", &showTracePanel);

    // --- render mode ------------------------------------------------------
    ImGui::TextUnformatted("Mode");
    ImGui::RadioButton("Ray cast (direct lighting only)",
                       &traceSettings.mode, RayTraceSettings::Mode_RayCast);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Primary rays only: find the nearest surface, shade it,\n"
                          "stop. No shadow, reflection or refraction rays.");
    }
    ImGui::RadioButton("Ray trace (recursive)",
                       &traceSettings.mode, RayTraceSettings::Mode_RayTrace);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Whitted-style: casts shadow rays and spawns reflected\n"
                          "and refracted rays up to the bounce limit.");
    }

    ImGui::Separator();
    ImGui::Text("Image");
    ImGui::InputInt("Width",  &traceSettings.width);
    ImGui::InputInt("Height", &traceSettings.height);
    if (traceSettings.width  < 1) traceSettings.width  = 1;
    if (traceSettings.height < 1) traceSettings.height = 1;

    if (ImGui::Button("Match viewport")) {
        // Different aspect ratios reframe the shot, which reads as "the render
        // does not match the scene". Copying the framebuffer size keeps them
        // consistent.
        int fbw = 0, fbh = 0;
        glfwGetFramebufferSize(glfwGetCurrentContext(), &fbw, &fbh);
        if (fbw > 0 && fbh > 0) { traceSettings.width = fbw; traceSettings.height = fbh; }
    }

    ImGui::DragInt("Samples/px", &traceSettings.samplesPerPixel, 0.05f, 1, 4, "%d", kTypeable);
    ImGui::TextDisabled("%d ray%s per pixel",
                        traceSettings.samplesPerPixel * traceSettings.samplesPerPixel,
                        traceSettings.samplesPerPixel > 1 ? "s" : "");

    ImGui::Separator();
    ImGui::Text("Effects");

    const bool recursiveMode = (traceSettings.mode == RayTraceSettings::Mode_RayTrace);
    if (!recursiveMode) ImGui::BeginDisabled();

    ImGui::Checkbox("Shadows", &traceSettings.shadows);
    ImGui::Checkbox("Reflections", &traceSettings.reflections);
    ImGui::Checkbox("Refraction", &traceSettings.refractions);

    if (!traceSettings.shadows) ImGui::BeginDisabled();
    ImGui::Checkbox("Transparent shadows", &traceSettings.transparentShadows);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Let shadow rays pass through transparent surfaces,\n"
                          "dimmed rather than stopped. Off = glass shadows like\n"
                          "a brick and objects behind it lose their highlights.");
    }
    if (!traceSettings.shadows) ImGui::EndDisabled();
    ImGui::DragInt("Max bounces", &traceSettings.maxDepth, 0.1f, 0, 10, "%d", kTypeable);

    if (!recursiveMode) {
        ImGui::EndDisabled();
        ImGui::TextWrapped("Ray cast mode casts no secondary rays, so these have "
                           "no effect. Switch to Ray trace to use them.");
    } else {
        // These toggles are easy to mistake for broken: with default materials
        // there is nothing for them to act on, so the image is byte-identical
        // either way. Say so explicitly rather than letting it look like a bug.
        int reflectiveCount = 0, transparentCount = 0;
        std::vector<Shape*>& allShapes = shapeManager.getShapes();
        for (size_t i = 0; i < allShapes.size(); ++i) {
            if (!allShapes[i]) continue;
            if (allShapes[i]->getReflectivity() > 0.0f) ++reflectiveCount;
            if (allShapes[i]->getTransparency() > 0.0f) ++transparentCount;
        }

        const ImVec4 warn(1.0f, 0.8f, 0.3f, 1.0f);
        if (traceSettings.reflections && reflectiveCount == 0) {
            ImGui::TextColored(warn, "No shape has Reflectivity > 0.");
        }
        if (traceSettings.refractions && transparentCount == 0) {
            ImGui::TextColored(warn, "No shape has Transparency > 0.");
        }
        if (traceSettings.shadows && allShapes.size() < 2) {
            ImGui::TextColored(warn, "Only %d object in scene - a shadow needs "
                                     "something to fall on.",
                               static_cast<int>(allShapes.size()));
        }
    }
    ImGui::ColorEdit3("Background", &traceSettings.background[0]);

    ImGui::Separator();
    ImGui::Text("Lights");
    {
        int lightCount = static_cast<int>(shapeManager.getLights().size());
        if (lightCount == 0) {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f),
                               "No lights in scene - using the built-in fallback.");
        } else {
            ImGui::TextDisabled("%d light%s. Select one to edit it.",
                                lightCount, lightCount == 1 ? "" : "s");
        }
        ImGui::TextDisabled("Insert > Light adds another.");
    }

    ImGui::Separator();

    // Material editing lives in the Shape Selector panel, next to the other
    // per-object properties, rather than being duplicated here.

    ImGui::Separator();

    if (rayTracer.isRendering()) {
        ImGui::ProgressBar(rayTracer.progress(), ImVec2(-1, 0));

        // Throughput and a remaining-time estimate.
        //
        // Frame rate is the wrong measure of a render: the loop hands the
        // tracer a fixed 20 ms slice per frame, so FPS stays near 50 no matter
        // how fast it is actually going. Rays per second is the real number,
        // and it is what differs between machines - the tracer is pure CPU.
        const float  done    = rayTracer.progress();
        const double elapsed = rayTracer.elapsedSeconds();
        if (done > 0.001f && elapsed > 0.0) {
            const double totalRays = double(rayTracer.getWidth()) * rayTracer.getHeight()
                                   * traceSettings.samplesPerPixel * traceSettings.samplesPerPixel;
            const double raysPerSec = (totalRays * done) / elapsed;
            const double remaining  = elapsed * (1.0 - done) / done;
            ImGui::TextDisabled("%.0f rays/s   ~%.0f s left", raysPerSec, remaining);
        }

        if (ImGui::Button("Cancel")) {
            rayTracer.cancel();
        }
    } else {
        if (ImGui::Button("Render", ImVec2(120, 0))) {
            lastSaveMessage.clear();
            rayTracer.begin(camera, shapeManager, traceSettings);
        }
    }

    if (rayTracer.hasImage()) {
        ImGui::SameLine();
        ImGui::Checkbox("Overlay viewport", &overlayPreview);

        ImGui::Text("%d x %d, %.2f s, %lu triangles",
                    rayTracer.getWidth(), rayTracer.getHeight(),
                    rayTracer.elapsedSeconds(),
                    static_cast<unsigned long>(rayTracer.getTriangleCount()));

        if (ImGui::Button("Save image...")) {
            const char* filters[2] = { "*.ppm", "*.bmp" };
            const char* path = tinyfd_saveFileDialog("Save rendered image",
                                                     "render.bmp", 2, filters,
                                                     "Image files");
            if (path) {
                lastSaveMessage = rayTracer.saveImage(path)
                                ? std::string("Saved: ") + path
                                : std::string("Failed to save: ") + path;
            }
        }

        if (!lastSaveMessage.empty()) {
            ImGui::TextWrapped("%s", lastSaveMessage.c_str());
        }

        // Inline preview, scaled to fit the panel width.
        unsigned int tex = rayTracer.getPreviewTexture();
        if (tex != 0) {
            float avail = ImGui::GetContentRegionAvail().x;
            if (avail > 16.0f) {
                float aspect = static_cast<float>(rayTracer.getHeight())
                             / static_cast<float>(rayTracer.getWidth());
                ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(tex)),
                             ImVec2(avail, avail * aspect));
            }
        }
    }

    endLeftPanel();
}


void Renderer::drawPreviewOverlay() {
    if (!overlayPreview || !rayTracer.hasImage()) return;

    unsigned int tex = rayTracer.getPreviewTexture();
    if (tex == 0) return;

    // Drawn through ImGui's BACKGROUND draw list: ImGui renders after the 3D
    // scene, so this still covers the viewport, but the control panels stay on
    // top of it. The foreground list would bury the panel that owns the toggle.
    ImGuiIO& io = ImGui::GetIO();
    float screenW = io.DisplaySize.x;
    float screenH = io.DisplaySize.y;
    if (screenW <= 0.0f || screenH <= 0.0f) return;

    // Letterbox to preserve the render's aspect ratio.
    float imgAspect    = static_cast<float>(rayTracer.getWidth())
                       / static_cast<float>(rayTracer.getHeight());
    float screenAspect = screenW / screenH;

    ImVec2 size = (imgAspect > screenAspect)
                ? ImVec2(screenW, screenW / imgAspect)
                : ImVec2(screenH * imgAspect, screenH);

    ImVec2 topLeft((screenW - size.x) * 0.5f, (screenH - size.y) * 0.5f);
    ImVec2 bottomRight(topLeft.x + size.x, topLeft.y + size.y);

    ImGui::GetBackgroundDrawList()->AddImage(
        reinterpret_cast<ImTextureID>(static_cast<intptr_t>(tex)),
        topLeft, bottomRight);
}


// ---------------------------------------------------------------------------
// File > Options
// ---------------------------------------------------------------------------

void Renderer::drawOptionsWindow() {
    if (!showOptionsWindow) return;

    ImGui::SetNextWindowSize(ImVec2(430, 0), ImGuiCond_FirstUseEver);
    ImGui::Begin("Options", &showOptionsWindow, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::TextUnformatted("Identity");
    ImGui::Separator();

    {
        // ImGui needs a fixed buffer; sync it from the setting each frame so
        // external edits to editor.cfg are picked up too.
        static char nameBuf[128];
        std::strncpy(nameBuf, g_settings.studentName.c_str(), sizeof(nameBuf) - 1);
        nameBuf[sizeof(nameBuf) - 1] = '\0';
        if (ImGui::InputText("Your name", nameBuf, sizeof(nameBuf))) {
            g_settings.studentName = nameBuf;
            updateWindowTitle();      // reflect it immediately
        }
        ImGui::TextDisabled("Appears in the window title.");

        static char shapeBuf[64];
        std::strncpy(shapeBuf, g_settings.customShapeName.c_str(), sizeof(shapeBuf) - 1);
        shapeBuf[sizeof(shapeBuf) - 1] = '\0';
        if (ImGui::InputText("Custom shape", shapeBuf, sizeof(shapeBuf))) {
            g_settings.customShapeName = shapeBuf;
            if (!g_settings.customShapeName.empty()) {
                customShapeName = g_settings.customShapeName;
            }
        }
        ImGui::TextDisabled("Label for the Custom shape in the Insert menu.");
    }

    ImGui::Spacing();
    ImGui::TextUnformatted("Scenes");
    ImGui::Separator();
    {
        static char folderBuf[256];
        std::strncpy(folderBuf, g_settings.sceneFolder.c_str(), sizeof(folderBuf) - 1);
        folderBuf[sizeof(folderBuf) - 1] = '\0';
        if (ImGui::InputText("Scene folder", folderBuf, sizeof(folderBuf))) {
            g_settings.sceneFolder = folderBuf;
        }
        ImGui::TextDisabled("Where Open and Save start. Created on first save.");
    }

    ImGui::Spacing();
    ImGui::TextUnformatted("Rendering backend");
    ImGui::Separator();

    // --- windowing platform -------------------------------------------------
    const char* platformLabels[3] = {
        "Automatic (recommended)",
        "X11",
        "Wayland"
    };

    bool anySelectable = SettingsIO::platformSupported(AppSettings::Platform_X11) ||
                         SettingsIO::platformSupported(AppSettings::Platform_Wayland);

    if (!anySelectable) {
        ImGui::TextDisabled("Windowing platform: not selectable in this build.");
        ImGui::TextDisabled("(Windows/macOS, or GLFW older than 3.4.)");
    } else {
        if (ImGui::BeginCombo("Windowing platform", platformLabels[g_settings.platform])) {
            for (int i = 0; i < 3; ++i) {
                const bool supported = SettingsIO::platformSupported(i);
                if (!supported) ImGui::BeginDisabled();

                if (ImGui::Selectable(platformLabels[i], g_settings.platform == i)) {
                    g_settings.platform = i;
                }

                if (!supported) {
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::TextDisabled("(unavailable)");
                }
            }
            ImGui::EndCombo();
        }

        if (SettingsIO::isRunningUnderWSL()) {
            ImGui::TextWrapped(
                "Running under WSL. Automatic selects X11, because WSLg's Wayland "
                "compositor does not resize the drawing surface when the window is "
                "maximized - the area outside the original window size is left "
                "uncleared.");
        }
    }

    ImGui::Spacing();

    // --- software rendering -------------------------------------------------
    ImGui::Checkbox("Force software rendering", &g_settings.softwareRendering);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Uses Mesa's llvmpipe rasterizer instead of the GPU driver.\n"
                          "Slow, but useful for telling driver bugs apart from\n"
                          "application bugs. Linux/Mesa only.");
    }

    ImGui::Spacing();
    ImGui::Separator();

    // --- what is actually running right now ---------------------------------
    ImGui::TextUnformatted("Currently active");
    ImGui::BulletText("Platform: %s", SettingsIO::activePlatformName());

    const GLubyte* glRenderer = glGetString(GL_RENDERER);
    const GLubyte* glVersion  = glGetString(GL_VERSION);
    ImGui::BulletText("Renderer: %s", glRenderer ? (const char*)glRenderer : "unknown");
    ImGui::BulletText("OpenGL:   %s", glVersion  ? (const char*)glVersion  : "unknown");

    ImGui::Separator();
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f),
                       "Changes take effect the next time the editor starts.");

    if (ImGui::Button("Save", ImVec2(110, 0))) {
        optionsSaveMessage = SettingsIO::save(g_settings, SettingsIO::defaultPath())
                           ? std::string("Saved to ") + SettingsIO::defaultPath()
                           : std::string("Could not write ") + SettingsIO::defaultPath();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset to defaults", ImVec2(150, 0))) {
        g_settings = AppSettings();
        optionsSaveMessage.clear();
    }

    if (!optionsSaveMessage.empty()) {
        ImGui::TextWrapped("%s", optionsSaveMessage.c_str());
    }

    ImGui::End();
}


// ---------------------------------------------------------------------------
// Material / light properties for the current selection
// ---------------------------------------------------------------------------

void Renderer::drawSelectedObjectProperties(ShapeManager& shapeManager) {
    Shape* selected = shapeManager.getSelectedShape();
    if (!selected) return;

    // --- lights get light controls instead of material controls -------------
    if (Light* light = dynamic_cast<Light*>(selected)) {
        if (!ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) return;
        ImGui::PushID("lightSection");
        struct PopGuard { ~PopGuard() { ImGui::PopID(); } } lightPop;

        bool enabled = light->isEnabled();
        if (ImGui::Checkbox("Enabled", &enabled)) light->setEnabled(enabled);

        int type = light->getType();
        if (ImGui::RadioButton("Point", &type, Light::Point))       light->setType(type);
        ImGui::SameLine();
        if (ImGui::RadioButton("Directional", &type, Light::Directional)) light->setType(type);

        glm::vec3 color = light->getColor();
        if (ImGui::ColorEdit3("Colour", &color[0])) light->setColor(color);

        float intensity = light->getIntensity();
        if (ImGui::DragFloat("Intensity", &intensity, 0.02f, 0.0f, 20.0f)) {
            light->setIntensity(intensity);
        }

        if (light->getType() == Light::Directional) {
            glm::vec3 dir = light->getDirection();
            if (ImGui::DragFloat3("Direction", &dir[0], 0.02f)) light->setDirection(dir);
            ImGui::TextDisabled("Directional lights ignore position and falloff.");
        }

        if (ImGui::TreeNode("Contribution")) {
            float a = light->getAmbientScale();
            float d = light->getDiffuseScale();
            float sp = light->getSpecularScale();
            if (ImGui::DragFloat("Ambient",  &a,  0.005f, 0.0f, 1.0f, "%.3f", kTypeable)) light->setAmbientScale(a);
            if (ImGui::DragFloat("Diffuse",  &d,  0.005f, 0.0f, 2.0f, "%.3f", kTypeable)) light->setDiffuseScale(d);
            if (ImGui::DragFloat("Specular", &sp, 0.005f, 0.0f, 2.0f, "%.3f", kTypeable)) light->setSpecularScale(sp);
            ImGui::TreePop();
        }

        if (light->getType() == Light::Point && ImGui::TreeNode("Attenuation")) {
            float c = light->getConstantAttenuation();
            float l = light->getLinearAttenuation();
            float q = light->getQuadraticAttenuation();
            ImGui::TextDisabled("1 / (c + l*d + q*d*d)");
            if (ImGui::DragFloat("Constant",  &c, 0.01f, 0.0f, 10.0f)) light->setConstantAttenuation(c);
            if (ImGui::DragFloat("Linear",    &l, 0.005f, 0.0f, 5.0f)) light->setLinearAttenuation(l);
            if (ImGui::DragFloat("Quadratic", &q, 0.005f, 0.0f, 5.0f)) light->setQuadraticAttenuation(q);
            if (c == 1.0f && l == 0.0f && q == 0.0f) {
                ImGui::TextDisabled("No falloff: distance does not affect brightness.");
            }
            ImGui::TreePop();
        }

        int lightCount = static_cast<int>(shapeManager.getLights().size());
        ImGui::TextDisabled("%d light%s in scene (shader limit 8)",
                            lightCount, lightCount == 1 ? "" : "s");
        return;
    }

    // --- everything else gets material controls -----------------------------
    if (!ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) return;
    // Scoped guard: this function has several early returns, and an unmatched
    // PushID corrupts the ID stack for every window drawn afterwards.
    ImGui::PushID("materialSection");
    struct PopGuard { ~PopGuard() { ImGui::PopID(); } } materialPop;

    Material& m = selected->getMaterial();

    ImGui::TextDisabled("Shown in the viewport and in renders");
    ImGui::DragFloat("Ambient (ka)",  &m.ambient,  0.005f, 0.0f, 1.0f, "%.3f", kTypeable);
    ImGui::DragFloat("Diffuse (kd)",  &m.diffuse,  0.005f, 0.0f, 1.0f, "%.3f", kTypeable);
    ImGui::DragFloat("Specular (ks)", &m.specular, 0.005f, 0.0f, 1.0f, "%.3f", kTypeable);
    ImGui::DragFloat("Shininess",     &m.shininess, 0.5f,   1.0f, 256.0f, "%.1f", kTypeable);

    ImGui::Spacing();
    ImGui::TextDisabled("Ray tracer only - no rasterized preview");
    ImGui::DragFloat("Reflectivity", &m.reflectivity, 0.005f, 0.0f, 1.0f, "%.3f", kTypeable);
    ImGui::DragFloat("Transparency", &m.transparency, 0.005f, 0.0f, 1.0f, "%.3f", kTypeable);
    ImGui::DragFloat("IOR", &m.indexOfRefraction, 0.005f, 1.0f, 2.5f, "%.3f", kTypeable);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Index of refraction (glass ~1.5, water ~1.33)");

    // --- texture -----------------------------------------------------------
    ImGui::Spacing();
    ImGui::TextDisabled("Texture");

    if (!selected->hasTexCoords()) {
        // Worth saying plainly. A shape with no parameterisation silently
        // ignoring an assigned texture is exactly the kind of thing that
        // costs a student an evening.
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f),
                           "This shape has no texture coordinates.");
        ImGui::TextDisabled("Sphere, Cube and OBJ meshes with vt lines have them.");
    }

    if (m.texturePath.empty()) {
        ImGui::TextDisabled("(none)");
    } else {
        // Tail of the path: the full thing is usually wider than the panel.
        std::string shown = m.texturePath;
        size_t slash = shown.find_last_of("/\\");
        if (slash != std::string::npos) shown = shown.substr(slash + 1);
        ImGui::TextWrapped("%s", shown.c_str());
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", m.texturePath.c_str());

        const Texture* t = TextureCache::get(m.texturePath);
        if (t && t->valid()) {
            ImGui::TextDisabled("%d x %d", t->width, t->height);
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.35f, 1.0f), "could not be loaded");
        }

        // The colour is a tint, and a saturated one hides the texture almost
        // entirely. Say so where it can be seen, rather than leaving someone
        // to conclude the mapping is broken.
        const glm::vec3 tint = selected->getBaseColor();
        if (tint.r < 0.92f || tint.g < 0.92f || tint.b < 0.92f) {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f),
                               "Tinted by the shape colour.");
            ImGui::SameLine();
            if (ImGui::SmallButton("Set white")) {
                selected->setColor(31);
                selected->setCustomColor(1.0f, 1.0f, 1.0f);
                sceneModified = true;
            }
        }
    }

    if (ImGui::Button("Choose texture...")) {
        const char* filters[] = { "*.png", "*.jpg", "*.jpeg", "*.tga", "*.bmp" };

        // Open where the sample textures are, unless a texture is already
        // assigned -- then reopen beside that one, which is where the next
        // texture almost always lives.
        //
        // The TRAILING SLASH matters: tinyfiledialogs treats the default as a
        // path to a FILE, so "data/textures" opens the parent directory with
        // "textures" typed into the filename box. With the slash it opens the
        // folder itself, which is what you want.
        std::string startIn;
        if (!m.texturePath.empty()) {
            const size_t slash = m.texturePath.find_last_of("/\\");
            if (slash != std::string::npos) {
                startIn = m.texturePath.substr(0, slash + 1);
            }
        }
        if (startIn.empty()) {
            FileImporter fi;
            const std::string exeDir = fi.getExecutableDirectory();
            startIn = (exeDir.empty() ? std::string(".") : exeDir)
                    + "/data/textures/";
        }

        const char* picked = tinyfd_openFileDialog(
            "Select a texture", startIn.c_str(), 5, filters,
            "Images (png, jpg, tga, bmp)", 0);
        if (picked) {
            m.texturePath = picked;

            // Switch to white unless the shape is already carrying a colour
            // the user chose deliberately.
            //
            // The shader multiplies the texture by material.color, which is
            // the standard modulate and lets a colour tint a greyscale map.
            // It also means assigning a texture to a shape on a saturated
            // preset produces something unrecognisable: a red cube under a
            // blue-and-green world map goes almost black, because red times
            // blue is nothing. That reads as "texturing is broken" rather
            // than "the tint is wrong", which is the wrong lesson.
            //
            // Preset colours are the editor's default state rather than a
            // decision, so those are replaced with white. A custom colour was
            // picked on purpose, so it is left alone and tints the texture.
            if (selected->getColorIndex() != 31) {
                selected->setColor(31);
                selected->setCustomColor(1.0f, 1.0f, 1.0f);
            }
            sceneModified = true;
        }
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(m.texturePath.empty());
    if (ImGui::Button("Clear texture")) {
        m.texturePath.clear();
        sceneModified = true;
    }
    ImGui::EndDisabled();

    if (ImGui::Button("Reset material")) selected->resetMaterial();
}


// ---------------------------------------------------------------------------
// File menu: New / Open / Open Recent / Save / Save As
// ---------------------------------------------------------------------------

void Renderer::updateWindowTitle() {
    std::string name = currentScenePath.empty() ? std::string("Untitled")
                                                : currentScenePath;
    size_t slash = name.find_last_of("/\\");
    if (slash != std::string::npos) name = name.substr(slash + 1);

    // Course and application names are fixed in Globals.cpp; the student name
    // comes from Options so each person sees their own without editing code.
    std::string title = courseName + " - " + appName;
    if (!g_settings.studentName.empty()) title += " - " + g_settings.studentName;
    title += " - " + name;
    if (sceneModified) title += " *";

    GLFWwindow* w = glfwGetCurrentContext();
    if (w) glfwSetWindowTitle(w, title.c_str());
}

void Renderer::newScene(ShapeManager& shapeManager) {
    std::vector<Shape*>& shapes = shapeManager.getShapes();
    for (size_t i = 0; i < shapes.size(); ++i) delete shapes[i];
    shapes.clear();
    shapeManager.setSelectedShape(0);
    shapeManager.setShapeCounter(0);

    // A scene with no light renders black, so start the way the app does.
    shapeManager.addShape(new Light(1.5f, 3.0f, 4.0f,
                                    shapeManager.incrementShapeCounter()));

    // "New" should hand back the application as it starts, not just an empty
    // shape list. Anything left over - a camera parked inside the old model, a
    // half-finished render, a simulation mid-flight - reads as a bug.

    // Camera back to the constructor's framing.
    camera.setView(3.14159265358979f / 2.0f, 0.0f, 5.0f, glm::vec3(0.0f));

    // Kill any render in progress and drop its preview. Leaving the panel up
    // showing the previous scene is worse than showing nothing.
    rayTracer.discardImage();
    traceSettings = RayTraceSettings();
    overlayPreview = false;

    // Stop the simulation and put the integrator back to its defaults, so a
    // diverged run does not carry its state into the new scene.
    TimeStepper& ts = Application::getTimeStepper();
    ts.stopAnimation();
    ts.resetAnimation();
    g_animation.reset(shapeManager, &ts);

    // Keyframe tracks reference shapes by id; a new scene has none of them.
    g_timeline.clear();

    // Panels closed, matching a fresh launch.
    showTracePanel     = false;
    showAnimationPanel = false;
    showOptionsWindow  = false;

    currentScenePath.clear();
    savedSignature = sceneSignature(shapeManager);
    sceneModified = false;
    signatureBaselined = true;
    sceneStatusMessage = "New scene";
    updateWindowTitle();
}

void Renderer::openScene(ShapeManager& shapeManager, const std::string& path) {
    SceneIO::Result r = SceneIO::load(path, shapeManager, camera, &traceSettings, &g_timeline);

    if (!r.ok) {
        sceneStatusMessage = r.error;
        return;
    }

    currentScenePath = path;
    savedSignature = sceneSignature(shapeManager);
    sceneModified = false;
    signatureBaselined = true;

    std::ostringstream msg;
    msg << "Loaded " << r.shapesLoaded << " object"
        << (r.shapesLoaded == 1 ? "" : "s");
    if (r.shapesSkipped > 0) msg << ", " << r.shapesSkipped << " skipped";
    for (size_t i = 0; i < r.warnings.size(); ++i) msg << "\n" << r.warnings[i];
    sceneStatusMessage = msg.str();

    SettingsIO::addRecentScene(g_settings, path);
    SettingsIO::save(g_settings, SettingsIO::defaultPath());
    updateWindowTitle();
}

bool Renderer::saveScene(ShapeManager& shapeManager, const std::string& path) {
    SceneIO::ensureFolderExists(SceneIO::defaultSceneFolder());

    if (!SceneIO::save(path, shapeManager, camera, &traceSettings, &g_timeline)) {
        sceneStatusMessage = "Could not write " + path;
        return false;
    }

    currentScenePath = path;
    savedSignature = sceneSignature(shapeManager);
    sceneModified = false;
    signatureBaselined = true;
    sceneStatusMessage = "Saved " + path;

    SettingsIO::addRecentScene(g_settings, path);
    SettingsIO::save(g_settings, SettingsIO::defaultPath());
    updateWindowTitle();
    return true;
}

void Renderer::promptSaveAs(ShapeManager& shapeManager) {
    SceneIO::ensureFolderExists(SceneIO::defaultSceneFolder());

    std::string suggested = SceneIO::defaultSceneFolder() + "/untitled.scene";
    if (!currentScenePath.empty()) suggested = currentScenePath;

    const char* filters[1] = { "*.scene" };
    const char* chosen = tinyfd_saveFileDialog("Save Scene", suggested.c_str(),
                                               1, filters, "Editor scene (*.scene)");
    if (chosen) saveScene(shapeManager, chosen);
}

void Renderer::promptOpen(ShapeManager& shapeManager) {
    SceneIO::ensureFolderExists(SceneIO::defaultSceneFolder());

    std::string startIn = SceneIO::defaultSceneFolder() + "/";
    const char* filters[1] = { "*.scene" };
    const char* chosen = tinyfd_openFileDialog("Open Scene", startIn.c_str(),
                                               1, filters,
                                               "Editor scene (*.scene)", 0);
    if (chosen) openScene(shapeManager, chosen);
}

void Renderer::drawFileMenu(ShapeManager& shapeManager) {
    if (!ImGui::BeginMenu("File")) return;

    if (ImGui::MenuItem("New", "Ctrl+N")) {
        if (sceneModified) {
            pendingSceneAction = 1;
            confirmDiscardOpen = true;
        } else {
            newScene(shapeManager);
        }
    }

    if (ImGui::MenuItem("Open...", "Ctrl+O")) {
        if (sceneModified) {
            pendingSceneAction = 2;
            pendingScenePath.clear();
            confirmDiscardOpen = true;
        } else {
            promptOpen(shapeManager);
        }
    }

    if (ImGui::BeginMenu("Open Recent", !g_settings.recentScenes.empty())) {
        for (size_t i = 0; i < g_settings.recentScenes.size(); ++i) {
            const std::string& entry = g_settings.recentScenes[i];

            // Show the filename, with the full path on hover.
            std::string label = entry;
            size_t slash = label.find_last_of("/\\");
            if (slash != std::string::npos) label = label.substr(slash + 1);

            if (ImGui::MenuItem(label.c_str())) {
                if (sceneModified) {
                    pendingSceneAction = 2;
                    pendingScenePath = entry;
                    confirmDiscardOpen = true;
                } else {
                    openScene(shapeManager, entry);
                }
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", entry.c_str());
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Clear Recent")) {
            g_settings.recentScenes.clear();
            SettingsIO::save(g_settings, SettingsIO::defaultPath());
        }
        ImGui::EndMenu();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Save", "Ctrl+S")) {
        if (currentScenePath.empty()) promptSaveAs(shapeManager);
        else                          saveScene(shapeManager, currentScenePath);
    }
    if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
        promptSaveAs(shapeManager);
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Options...")) {
        showOptionsWindow = true;
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Quit", "Esc")) {
        glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
    }

    ImGui::EndMenu();
}

void Renderer::drawSceneModals(ShapeManager& shapeManager) {
    if (confirmDiscardOpen) {
        ImGui::OpenPopup("Discard current scene?");
        confirmDiscardOpen = false;
    }

    if (ImGui::BeginPopupModal("Discard current scene?", NULL,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted("The current scene has unsaved changes.");
        ImGui::Spacing();

        if (ImGui::Button("Save first", ImVec2(110, 0))) {
            if (currentScenePath.empty()) promptSaveAs(shapeManager);
            else                          saveScene(shapeManager, currentScenePath);

            // Only continue if the save actually succeeded.
            if (!sceneModified) {
                if (pendingSceneAction == 1) newScene(shapeManager);
                else if (pendingScenePath.empty()) promptOpen(shapeManager);
                else openScene(shapeManager, pendingScenePath);
                pendingSceneAction = 0;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Discard", ImVec2(110, 0))) {
            if (pendingSceneAction == 1) newScene(shapeManager);
            else if (pendingScenePath.empty()) promptOpen(shapeManager);
            else openScene(shapeManager, pendingScenePath);
            pendingSceneAction = 0;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(110, 0))) {
            pendingSceneAction = 0;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Keyboard shortcuts, ignored while a text field has focus.
    ImGuiIO& io = ImGui::GetIO();
    if (io.KeyCtrl && !ImGui::IsAnyItemActive()) {
        if (ImGui::IsKeyPressed(ImGuiKey_S, false)) {
            if (io.KeyShift || currentScenePath.empty()) promptSaveAs(shapeManager);
            else saveScene(shapeManager, currentScenePath);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_O, false)) promptOpen(shapeManager);
        if (ImGui::IsKeyPressed(ImGuiKey_N, false)) newScene(shapeManager);
    }
}


// Content signature used for the unsaved-changes check.
//
// Covers exactly what save() writes for each object, so "modified" means the
// file on disk would differ. The camera is deliberately excluded: it is saved
// with the scene, but merely orbiting the view should not count as an edit
// worth blocking New or Open over.
unsigned long long Renderer::sceneSignature(ShapeManager& shapeManager) const {
    // FNV-1a.
    unsigned long long h = 1469598103934665603ULL;

    struct Mix {
        unsigned long long& h;
        void bytes(const void* p, size_t n) {
            const unsigned char* b = static_cast<const unsigned char*>(p);
            for (size_t i = 0; i < n; ++i) {
                h ^= b[i];
                h *= 1099511628211ULL;
            }
        }
        void f(float v)  { bytes(&v, sizeof(v)); }
        void i(int v)    { bytes(&v, sizeof(v)); }
        void s(const std::string& v) { bytes(v.data(), v.size()); i(0); }
    } mix = { h };

    // Keyframe tracks are saved with the scene, so they belong in the
    // signature too -- otherwise keying a shape and quitting loses the work
    // with no prompt.
    mix.i(static_cast<int>(g_timeline.tracks.size()));
    mix.f(g_timeline.length);
    for (size_t t = 0; t < g_timeline.tracks.size(); ++t) {
        const AnimationTrack& tr = g_timeline.tracks[t];
        mix.i(tr.shapeId);
        mix.i(tr.loop ? 1 : 0);
        for (size_t k = 0; k < tr.keys.size(); ++k) {
            const Keyframe& kf = tr.keys[k];
            mix.f(kf.time);
            mix.f(kf.position.x); mix.f(kf.position.y); mix.f(kf.position.z);
            mix.f(kf.rotation.x); mix.f(kf.rotation.y); mix.f(kf.rotation.z);
            mix.f(kf.scale.x);    mix.f(kf.scale.y);    mix.f(kf.scale.z);
        }
    }

    std::vector<Shape*>& shapes = shapeManager.getShapes();
    mix.i(static_cast<int>(shapes.size()));

    for (size_t k = 0; k < shapes.size(); ++k) {
        Shape* sh = shapes[k];
        if (!sh) continue;

        mix.s(sh->serialType());
        mix.s(sh->getShapeType());
        mix.s(sh->getSourcePath());
        mix.i(sh->getId());
        mix.f(sh->getX()); mix.f(sh->getY()); mix.f(sh->getZ());
        mix.f(sh->getAngleX()); mix.f(sh->getAngleY()); mix.f(sh->getAngleZ());
        mix.i(sh->isUsingUniformScaling() ? 1 : 0);
        mix.f(sh->getScale());
        glm::vec3 nu = sh->getNonUniformScale();
        mix.f(nu.x); mix.f(nu.y); mix.f(nu.z);
        mix.i(sh->getColorIndex());
        const float* cc = sh->getCustomColor();
        mix.f(cc[0]); mix.f(cc[1]); mix.f(cc[2]);

        const Material& m = sh->getMaterial();
        mix.f(m.ambient); mix.f(m.diffuse); mix.f(m.specular); mix.f(m.shininess);
        mix.f(m.reflectivity); mix.f(m.transparency); mix.f(m.indexOfRefraction);
        // Assigning or clearing a texture is a scene change like any other.
        // Left out, the editor would let you close without saving after
        // texturing everything.
        mix.s(m.texturePath);

        if (Light* L = dynamic_cast<Light*>(sh)) {
            mix.i(L->getType());
            mix.i(L->isEnabled() ? 1 : 0);
            mix.f(L->getColor().r); mix.f(L->getColor().g); mix.f(L->getColor().b);
            mix.f(L->getIntensity());
            mix.f(L->getAmbientScale()); mix.f(L->getDiffuseScale()); mix.f(L->getSpecularScale());
            mix.f(L->getDirection().x); mix.f(L->getDirection().y); mix.f(L->getDirection().z);
            mix.f(L->getConstantAttenuation());
            mix.f(L->getLinearAttenuation());
            mix.f(L->getQuadraticAttenuation());
        }
    }

    return h;
}


// ---------------------------------------------------------------------------
// Animation
// ---------------------------------------------------------------------------

void Renderer::drawAnimationMenu(ShapeManager& shapeManager) {
    if (!ImGui::BeginMenu("Animation")) return;

    TimeStepper& ts = Application::getTimeStepper();
    const bool playing = ts.isAnimationPlaying();

    // Two submenus rather than one flat list of transport entries.
    //
    // The menu used to expose only the particle transport, unlabelled, so
    // "Animation > Play" started the simulation and did nothing at all to a
    // keyframe track -- which reads as a bug to a 484 student, whose entire
    // assignment is keyframes. Naming each system fixes that without hiding
    // either one: both courses see both menus and can tell at a glance which
    // is theirs.
    if (ImGui::BeginMenu("Particle Systems")) {
        // One toggle rather than separate Play and Stop entries with duelling
        // checkmarks: it is a single boolean, so it should look like one.
        if (ImGui::MenuItem(playing ? "Pause" : "Play", "Space")) {
            if (playing) ts.stopAnimation();
            else         ts.playAnimation();
        }

        if (ImGui::MenuItem("Step Forward", "Right", false, !playing)) {
            g_animation.requestSingleStep();
        }

        if (ImGui::MenuItem("Reset", "R")) {
            g_animation.reset(shapeManager, &Application::getTimeStepper());
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Keyframes")) {
        if (ImGui::MenuItem(g_timeline.playing ? "Pause" : "Play")) {
            g_timeline.playing = !g_timeline.playing;
        }

        // No Step entry to match the particle menu's: a keyframe track is a
        // pure function of t, so stepping it is just moving the playhead, and
        // the panel's scrub control does that better than a menu item can.
        if (ImGui::MenuItem("Rewind")) {
            g_timeline.reset(shapeManager);
        }

        ImGui::MenuItem("Loop", NULL, &g_timeline.loopAll);

        ImGui::EndMenu();
    }

    ImGui::Separator();

    // Both at once, matching the panel's Play All / Reset All buttons. These
    // do not synchronise the two systems and cannot: particles advance by a
    // fixed integration step per frame, keyframes by real elapsed time.
    const bool anyPlaying = playing || g_timeline.playing;
    if (ImGui::MenuItem(anyPlaying ? "Pause All" : "Play All")) {
        if (anyPlaying) {
            ts.stopAnimation();
            g_timeline.playing = false;
        } else {
            ts.playAnimation();
            g_timeline.playing = true;
        }
    }

    if (ImGui::MenuItem("Reset All")) {
        g_animation.reset(shapeManager, &Application::getTimeStepper());
        g_timeline.reset(shapeManager);
    }

    ImGui::Separator();

    ImGui::MenuItem("Animation Panel", NULL, &showAnimationPanel);

    ImGui::EndMenu();
}


void Renderer::drawAnimationPanel(ShapeManager& shapeManager) {
    // Shortcuts work whether or not the panel is open.
    ImGuiIO& io = ImGui::GetIO();
    if (!ImGui::IsAnyItemActive() && !io.KeyCtrl) {
        TimeStepper& ts = Application::getTimeStepper();
        if (ImGui::IsKeyPressed(ImGuiKey_Space, false)) {
            if (ts.isAnimationPlaying()) ts.stopAnimation();
            else                         ts.playAnimation();
        }
        if (ImGui::IsKeyPressed(ImGuiKey_RightArrow, false) && !ts.isAnimationPlaying()) {
            g_animation.requestSingleStep();
        }
        if (ImGui::IsKeyPressed(ImGuiKey_R, false)) {
            g_animation.reset(shapeManager, &Application::getTimeStepper());
        }
    }

    if (!showAnimationPanel) return;

    beginLeftPanel("Animation", &showAnimationPanel);

    TimeStepper& ts = Application::getTimeStepper();
    const bool playing = ts.isAnimationPlaying();

    const size_t systemCount = shapeManager.getParticleSystems().size();
    if (systemCount == 0) {
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f),
                           "No particle systems in the scene.");
        ImGui::TextDisabled("Insert > Pendulum / Chain / Cloth to simulate something.");
        ImGui::Separator();
    }

    // --- everything at once ------------------------------------------------
    //
    // Two independent animation systems live in this panel, and a student
    // normally has only one of them: 484 does keyframes, 566 does particle
    // systems. Without a single obvious transport, both are faced with two
    // Play buttons and no indication which is theirs.
    //
    // These start and zero BOTH. They do not synchronise them, and cannot:
    // the particle systems advance by substeps x stepSize x speed per frame
    // (fixed-step integration), while the keyframe timeline advances by real
    // elapsed seconds. Started together they will drift apart.
    {
        const bool anyPlaying = playing || g_timeline.playing;
        if (ImGui::Button(anyPlaying ? "Pause All" : "Play All", ImVec2(136, 0))) {
            if (anyPlaying) {
                ts.stopAnimation();
                g_timeline.playing = false;
            } else {
                ts.playAnimation();
                g_timeline.playing = true;
            }
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Starts the particle simulation and the keyframe\n"
                              "timeline together.\n\n"
                              "They do not stay in step: particles advance by a\n"
                              "fixed integration step per frame, keyframes by\n"
                              "real elapsed time.");
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset All", ImVec2(136, 0))) {
            g_animation.reset(shapeManager, &ts);
            g_timeline.reset(shapeManager);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Returns every particle system to its initial state\n"
                              "and the keyframe playhead to t = 0.");
        }
    }

    // --- the two animation systems -----------------------------------------
    //
    // Two systems, two headers, named for what they are. 484 does keyframes,
    // 566 does particle systems, and a student who sees only "Play" twice has
    // no way to tell which transport is the one their assignment drives.
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Particle Systems", ImGuiTreeNodeFlags_DefaultOpen)) {
        drawParticleSection(shapeManager);
    }

    // --- keyframe timeline -------------------------------------------------
    //
    // Deliberately in the same panel as the particle transport, and
    // deliberately below it: they are two different animation systems and
    // students should see that they are separate. The particle systems
    // integrate forwards and cannot be scrubbed; a keyframe track is a pure
    // function of t, so this slider can go anywhere including backwards.
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Keyframes", ImGuiTreeNodeFlags_DefaultOpen)) {
        drawKeyframeSection(shapeManager);
    }

    endLeftPanel();
}

void Renderer::drawParticleSection(ShapeManager& shapeManager) {
    TimeStepper& ts   = Application::getTimeStepper();
    const bool playing = ts.isAnimationPlaying();
    const size_t systemCount = shapeManager.getParticleSystems().size();

    if (ImGui::Button(playing ? "Pause" : "Play", ImVec2(90, 0))) {
        if (playing) ts.stopAnimation();
        else         ts.playAnimation();
    }
    ImGui::SameLine();

    ImGui::BeginDisabled(playing);
    if (ImGui::Button("Step", ImVec2(90, 0))) g_animation.requestSingleStep();
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Reset", ImVec2(90, 0))) {
        g_animation.reset(shapeManager, &Application::getTimeStepper());
    }

    ImGui::TextDisabled("Space = play/pause, Right = step, R = reset");

    ImGui::Separator();

    // --- clock -------------------------------------------------------------
    ImGui::Text("Time  %.3f s", g_animation.simTime);
    ImGui::Text("Steps %ld", g_animation.stepCount);
    ImGui::Text("Systems %d", static_cast<int>(systemCount));

    ImGui::Separator();

    // --- integrator --------------------------------------------------------
    const char* integratorNames[] = { "Forward Euler", "Midpoint", "Trapezoidal", "RK4" };
    int current = g_animation.integrator;
    if (ImGui::Combo("Integrator", &current, integratorNames, 4)) {
        g_animation.integrator = current;
        selectedIntegrator = static_cast<IntegratorType>(current);
        // Preserve the step size across the swap; createIntegrator starts fresh.
        float keep = ts.getStepSize();
        bool wasPlaying = ts.isAnimationPlaying();
        Application::setTimeStepper(
            TimeStepper::createIntegrator(static_cast<IntegratorType>(current)));
        Application::getTimeStepper().setStepSize(keep);
        if (wasPlaying) Application::getTimeStepper().playAnimation();
    }

    if (g_animation.integrator == static_cast<int>(IntegratorType::ForwardEuler)) {
        ImGui::TextDisabled("Forward Euler is the least stable; it diverges first.");
    }

    // --- step size ---------------------------------------------------------
    float step = ts.getStepSize();
    if (ImGui::DragFloat("Step size", &step, 0.0001f, 0.0001f, 0.05f, "%.4f s",
                         kTypeable | ImGuiSliderFlags_Logarithmic)) {
        ts.setStepSize(step);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Smaller is more stable and slower.\n"
                          "Logarithmic, so the useful low end is reachable.");
    }

    ImGui::DragInt("Substeps", &g_animation.substeps, 0.1f, 1, 20, "%d", kTypeable);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Integration steps per rendered frame. Raises simulation\n"
                          "rate without enlarging the step size.");
    }

    ImGui::DragFloat("Speed", &g_animation.speed, 0.01f, 0.0f, 4.0f, "%.2fx", kTypeable);

    float effective = g_animation.substeps * g_animation.speed * ts.getStepSize();
    ImGui::TextDisabled("~%.3f simulated seconds per frame", effective);

    ImGui::Separator();

    // --- stability ---------------------------------------------------------
    ImGui::Checkbox("Pause if unstable", &g_animation.guardEnabled);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Stops when a particle goes NaN or flies off.\n"
                          "Without it, a diverging solve just makes the scene vanish.");
    }
    if (g_animation.guardEnabled) {
        ImGui::DragFloat("Limit", &g_animation.guardLimit, 100.0f, 10.0f, 1.0e9f, "%.0f");
    }

    if (g_animation.diverged && !g_animation.status.empty()) {
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.45f, 0.35f, 1.0f));
        ImGui::TextWrapped("%s", g_animation.status.c_str());
        ImGui::PopStyleColor();
        if (ImGui::Button("Reset and clear")) {
            g_animation.reset(shapeManager, &Application::getTimeStepper());
        }
    }
}

// The frame the step buttons move by.
//
// Nothing else in the editor defines a frame rate -- the viewport runs at
// whatever the display gives it and the particle systems integrate on their own
// step size -- so this invents one purely as a scrub granularity. 1/30 s is
// small enough to land between two close keys and large enough that holding the
// button visibly moves.
static const float kTimelineFrame = 1.0f / 30.0f;

// ---------------------------------------------------------------------------
// The keyframe timeline strip
// ---------------------------------------------------------------------------
//
// Replaces the time slider that used to live here. A slider shows one number;
// this shows the same number IN CONTEXT -- where the keys are, which track they
// belong to, and how far through the length the playhead has travelled -- and
// it is still the control that sets it. Drag anywhere on the ruler or on a
// track row and the playhead follows.
//
// Drawn by hand into the window's draw list rather than assembled from widgets.
// ImGui has no timeline control, and the alternative -- a row of buttons per
// track -- cannot show two keys 40 ms apart at all, which at this panel width
// is a real case.
//
// Keys are READ-ONLY here. Dragging them means hit-testing, a drag threshold,
// undo, and deciding what happens when two keys land on the same frame; all
// worth doing, and all worth doing separately once the strip itself is proven.
// The table below the strip is still where a key's values are read, and it is
// still clickable to jump the playhead.
void Renderer::drawTimelineStrip(ShapeManager& shapeManager) {
    // One row per track, at a height that leaves the diamonds room to be
    // distinguishable without the strip eating the panel.
    const float kRulerH = 16.0f;
    const float kRowH   = 20.0f;
    const float kEmptyH = 40.0f;

    // Beyond this the track area scrolls rather than growing. Six rows is
    // 120 px, which is about as much of a 360 px panel as the strip can take
    // before the key list and the keying buttons below it are pushed off the
    // bottom -- and those are what you reach for after reading the strip.
    const int kMaxVisibleRows = 6;

    // Only tracks that actually hold a key are shown. A shape earns its row by
    // being keyed, not by being selected, so the strip stays a picture of the
    // animation rather than of the scene. Empty tracks should not exist --
    // deleting a track's last key removes it, and the scene loader skips
    // keyless tracks on the way in -- but drawing depends on it, so it is
    // filtered here rather than assumed.
    std::vector<const AnimationTrack*> rows;
    rows.reserve(g_timeline.tracks.size());
    for (size_t i = 0; i < g_timeline.tracks.size(); ++i) {
        if (!g_timeline.tracks[i].keys.empty()) rows.push_back(&g_timeline.tracks[i]);
    }
    const int rowCount = static_cast<int>(rows.size());

    const float contentH = (rowCount > 0) ? (rowCount * kRowH) : kEmptyH;
    const bool  scrolls  = rowCount > kMaxVisibleRows;
    const float viewH    = scrolls ? (kMaxVisibleRows * kRowH) : contentH;

    float avail = ImGui::GetContentRegionAvail().x;
    if (avail < 60.0f) avail = 60.0f;   // a collapsed panel must not divide by ~0

    // The time axis is narrower than the panel exactly when a scrollbar is
    // present, and the RULER has to lose the same width even though it is not
    // inside the scrolling region -- otherwise its ticks drift out of line with
    // the keys they are labelling, which is the one thing a ruler cannot do.
    const float axisW = scrolls ? (avail - ImGui::GetStyle().ScrollbarSize) : avail;

    const float len = (g_timeline.length > 0.01f) ? g_timeline.length : 0.01f;
    const float pxPerSec = axisW / len;

    const ImU32 cBg      = IM_COL32(27, 30, 34, 255);
    const ImU32 cLine    = IM_COL32(56, 61, 69, 255);
    const ImU32 cRowLine = IM_COL32(42, 46, 52, 255);
    const ImU32 cSelRow  = IM_COL32(42, 50, 60, 255);
    const ImU32 cDim     = IM_COL32(138, 145, 156, 255);
    const ImU32 cText    = IM_COL32(200, 211, 224, 255);
    const ImU32 cKey     = IM_COL32(216, 178, 74, 255);
    const ImU32 cKeySel  = IM_COL32(242, 210, 122, 255);
    const ImU32 cPlay    = IM_COL32(224, 92, 74, 255);
    const ImU32 cShadow  = IM_COL32(15, 17, 20, 220);

    // Set from either interaction region below; applied once at the end so the
    // ruler and the rows cannot fight over the playhead within a frame.
    bool  scrubbed = false;
    float scrubTo  = 0.0f;

    // --- ruler --------------------------------------------------------------
    //
    // Outside the scrolling region on purpose: scrolling to the eighth track
    // must not take the time axis off the top of the strip with it.
    {
        const ImVec2 o = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton("##tlruler", ImVec2(avail, kRulerH));
        ImDrawList* dl = ImGui::GetWindowDrawList();

        dl->AddRectFilled(o, ImVec2(o.x + avail, o.y + kRulerH), cBg);

        // The tick interval adapts to the length so the labels never collide:
        // at 5 s and ~344 px a second is 68 px, which fits "0s"; at 60 s it
        // would not.
        float step = 1.0f;
        if (len <= 2.0f)       step = 0.25f;
        else if (len <= 6.0f)  step = 0.5f;
        else if (len <= 20.0f) step = 1.0f;
        else if (len <= 60.0f) step = 5.0f;
        else                   step = 10.0f;

        const float bottom = o.y + kRulerH;
        for (float t = 0.0f; t <= len + 1e-4f; t += step) {
            const float x = o.x + t * pxPerSec;
            // "Major" means a whole second, which is what gets a label. With a
            // sub-second step the intermediate ticks are drawn shorter so the
            // second boundaries stay readable at a glance.
            const bool major = std::fabs(t - std::floor(t + 0.5f)) < 1e-4f;
            dl->AddLine(ImVec2(x, bottom - (major ? 7.0f : 4.0f)), ImVec2(x, bottom),
                        major ? cLine : cRowLine);
            if (major) {
                char lbl[16];
                std::snprintf(lbl, sizeof(lbl), "%.0fs", t);
                // Skip a label that would run past the axis rather than letting
                // it clip to a stray digit -- the tick is still drawn, and "the
                // strip ends at the length" needs no label to say so.
                if (x + 2.0f + ImGui::CalcTextSize(lbl).x <= o.x + axisW) {
                    dl->AddText(ImVec2(x + 2.0f, o.y), cDim, lbl);
                }
            }
        }
        dl->AddLine(ImVec2(o.x, bottom), ImVec2(o.x + avail, bottom), cLine);

        const float px = o.x + g_timeline.currentTime * pxPerSec;
        dl->AddLine(ImVec2(px, o.y), ImVec2(px, bottom), cPlay, 1.0f);
        dl->AddTriangleFilled(ImVec2(px - 4.0f, o.y), ImVec2(px + 4.0f, o.y),
                              ImVec2(px, o.y + 6.0f), cPlay);

        if (ImGui::IsItemActive()) {
            scrubbed = true;
            scrubTo  = (ImGui::GetIO().MousePos.x - o.x) / pxPerSec;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Drag to scrub");
        }
    }

    // --- track rows ---------------------------------------------------------
    //
    // A child window so the rows scroll once there are more than fit. The
    // border is drawn by hand around the whole strip below rather than by the
    // child, so the ruler and the rows read as one control.
    int clickedRow = -1;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, cBg);
    ImGui::BeginChild("##tltracks", ImVec2(avail, viewH), false,
                      ImGuiWindowFlags_NoMove);
    {
        const ImVec2 o = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton("##tlrows", ImVec2(axisW, contentH));
        ImDrawList* dl = ImGui::GetWindowDrawList();

        Shape* sel = shapeManager.getSelectedShape();
        const int selId = sel ? sel->getId() : -1;

        if (rowCount == 0) {
            const char* msg = "No keyframed shapes yet";
            const ImVec2 sz = ImGui::CalcTextSize(msg);
            dl->AddText(ImVec2(o.x + (axisW - sz.x) * 0.5f, o.y + (kEmptyH - sz.y) * 0.5f),
                        cDim, msg);
        }

        for (int i = 0; i < rowCount; ++i) {
            const AnimationTrack& tr = *rows[i];
            const float top = o.y + i * kRowH;
            const float bot = top + kRowH;
            const bool isSel = (tr.shapeId == selId);

            if (isSel) {
                dl->AddRectFilled(ImVec2(o.x, top), ImVec2(o.x + axisW, bot), cSelRow);
            }
            dl->AddLine(ImVec2(o.x, bot), ImVec2(o.x + axisW, bot), cRowLine);

            // Keys first, name second: a name overlapping a diamond has to stay
            // readable, and the diamond under it is still findable by its
            // position on the ruler.
            for (size_t k = 0; k < tr.keys.size(); ++k) {
                const float x = o.x + tr.keys[k].time * pxPerSec;
                const float y = (top + bot) * 0.5f;
                const float r = 4.5f;
                const ImVec2 pts[4] = { ImVec2(x, y - r), ImVec2(x + r, y),
                                        ImVec2(x, y + r), ImVec2(x - r, y) };
                dl->AddQuadFilled(pts[0], pts[1], pts[2], pts[3], isSel ? cKeySel : cKey);
                dl->AddQuad(pts[0], pts[1], pts[2], pts[3], IM_COL32(107, 87, 32, 255));
            }

            // The shape may have been deleted while its track survived, so the
            // name falls back to the id rather than dereferencing whatever
            // comes back.
            Shape* s = shapeManager.getShapeById(tr.shapeId);
            char name[64];
            if (s) {
                std::snprintf(name, sizeof(name), "%s %d",
                              s->getShapeType().c_str(), tr.shapeId);
            } else {
                std::snprintf(name, sizeof(name), "(deleted %d)", tr.shapeId);
            }

            // A one-pixel dark outline in four directions: the draw list has no
            // text shadow, and without it a gold diamond directly behind a
            // letter makes the label unreadable.
            const ImVec2 at(o.x + 4.0f, top + 3.0f);
            dl->AddText(ImVec2(at.x - 1.0f, at.y), cShadow, name);
            dl->AddText(ImVec2(at.x + 1.0f, at.y), cShadow, name);
            dl->AddText(ImVec2(at.x, at.y - 1.0f), cShadow, name);
            dl->AddText(ImVec2(at.x, at.y + 1.0f), cShadow, name);
            dl->AddText(at, isSel ? cText : cDim, name);

            const bool pressedInRow =
                ImGui::IsItemClicked() &&
                ImGui::GetIO().MousePos.y >= top && ImGui::GetIO().MousePos.y < bot;

            if (pressedInRow) {
                clickedRow = i;

                // Grab the nearest diamond within kGrab pixels. Wider than the
                // diamond is drawn, because a 9 px target at 68 px per second
                // is a fussy thing to hit and the cost of a near miss is only
                // that you scrub instead.
                const float mx = ImGui::GetIO().MousePos.x;
                const float kGrab = 6.0f;
                float best = kGrab;
                int   bestKey = -1;
                for (size_t k = 0; k < tr.keys.size(); ++k) {
                    const float d = std::fabs((o.x + tr.keys[k].time * pxPerSec) - mx);
                    if (d <= best) { best = d; bestKey = static_cast<int>(k); }
                }

                if (bestKey >= 0) {
                    dragShapeId   = tr.shapeId;
                    dragKeyIndex  = bestKey;
                    dragStartTime = tr.keys[bestKey].time;
                    dragPressX    = mx;
                    dragArmed     = true;
                    dragActive    = false;
                }
            }
        }

        // Playhead over the rows, clipped to the child so it cannot run past
        // the visible area when the list is scrolled.
        const float px = o.x + g_timeline.currentTime * pxPerSec;
        dl->AddLine(ImVec2(px, o.y), ImVec2(px, o.y + contentH), cPlay, 1.0f);

        if (ImGui::IsItemActive()) {
            scrubbed = true;
            scrubTo  = (ImGui::GetIO().MousePos.x - o.x) / pxPerSec;
        }
        if (ImGui::IsItemHovered()) {
            const float t = (ImGui::GetIO().MousePos.x - o.x) / pxPerSec;
            ImGui::SetTooltip("t = %.2f s\nDrag to scrub, click a row to select it",
                              (t < 0.0f) ? 0.0f : ((t > len) ? len : t));
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    // One border around ruler and rows together.
    {
        const ImVec2 a(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y - kRulerH);
        const ImVec2 b(ImGui::GetItemRectMax());
        ImGui::GetWindowDrawList()->AddRect(a, b, cLine);
    }

    if (scrolls) {
        ImGui::TextDisabled("%d tracks -- scroll the strip for the rest", rowCount);
    }

    // --- apply the scrub ----------------------------------------------------
    //
    // Scrubbing stops playback: a playhead fighting the pointer is the single
    // most confusing thing a timeline can do. It applies the pose live rather
    // than on release, for the reason the old slider did -- a drag that does
    // nothing until you let go reads as broken.
    // --- the keyframe drag --------------------------------------------------
    //
    // Handled before the scrub, and it SUPPRESSES the scrub: a press that
    // grabbed a diamond is retiming that key, not moving the playhead. Doing
    // both at once would drag the key and chase it with the playhead, which
    // reads as the key running away from the pointer.
    bool draggingKey = false;
    if (dragArmed) {
        AnimationTrack* tr = g_timeline.findTrack(dragShapeId);

        // Escape cancels: put the key back where the press found it and let go.
        // The cheap alternative -- no cancel at all -- means the only way out of
        // a drag you regret is to complete it and then undo, and undo is a
        // whole-track snapshot, which is a lot of machinery to reach for
        // because you twitched.
        const bool cancel = ImGui::IsKeyPressed(ImGuiKey_Escape, false);

        if (!tr || dragKeyIndex < 0 ||
            static_cast<size_t>(dragKeyIndex) >= tr->keys.size()) {
            dragArmed = dragActive = false;      // the track went away under us
        } else if (cancel) {
            dragKeyIndex = tr->moveKey(static_cast<size_t>(dragKeyIndex), dragStartTime);
            dragArmed = dragActive = false;
            dragUndoShapeId = -1;                // nothing happened, nothing to undo
            g_timeline.apply(shapeManager);
        } else if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            const float mx = ImGui::GetIO().MousePos.x;

            // The threshold is what keeps a click on a diamond behaving as it
            // always did. Below it this is still a click; above it, it is a
            // drag and stays one until the button comes up.
            if (!dragActive && std::fabs(mx - dragPressX) > 4.0f) {
                dragActive = true;
                dragUndoShapeId = dragShapeId;
                dragUndoKeys    = tr->keys;      // snapshot before the first move
            }

            if (dragActive) {
                float t = dragStartTime + (mx - dragPressX) / pxPerSec;
                if (t < 0.0f) t = 0.0f;
                if (t > len)  t = len;

                // Snap to the frame grid unless Alt is held. Keys land on
                // frames far more often than not, and a key at 1.0167 s that
                // looks like 1.0 is the sort of thing you only discover from
                // the key table three edits later.
                if (!ImGui::GetIO().KeyAlt) {
                    t = std::floor(t / kTimelineFrame + 0.5f) * kTimelineFrame;
                }

                dragKeyIndex = tr->moveKey(static_cast<size_t>(dragKeyIndex), t);
                g_timeline.ensureLength(t);
                g_timeline.apply(shapeManager);
                sceneModified = true;
            }

            // Only a REAL drag suppresses the scrub. Suppressing from the press
            // instead means clicking a diamond does nothing at all -- the click
            // is swallowed as a drag that never happened, and the playhead
            // stays where it was.
            draggingKey = dragActive;
        } else {
            // Released. Park the playhead on the key that was just moved: it
            // shows the pose you were retiming, and it puts the existing
            // "Delete key" button on that key without inventing a second way to
            // delete one.
            if (dragActive && dragKeyIndex >= 0 &&
                static_cast<size_t>(dragKeyIndex) < tr->keys.size()) {
                g_timeline.currentTime = tr->keys[dragKeyIndex].time;
                g_timeline.playing = false;
                g_timeline.apply(shapeManager);
            }
            dragArmed = dragActive = false;
        }
    }

    if (scrubbed && !draggingKey) {
        if (scrubTo < 0.0f) scrubTo = 0.0f;
        if (scrubTo > len)  scrubTo = len;
        g_timeline.currentTime = scrubTo;
        g_timeline.playing = false;
        g_timeline.apply(shapeManager);
        sceneModified = true;
    }

    // Selecting the row's shape is deliberately a side effect of the click that
    // also scrubs: the row is the track, and the buttons below act on the
    // selected shape's track, so clicking a row and then finding the buttons
    // still pointed elsewhere would be the surprising behaviour.
    if (clickedRow >= 0) {
        shapeManager.setSelectedShapeById(rows[clickedRow]->shapeId);
    }

    // The grabbed key gets a ring, so it is obvious which one is moving when
    // several sit close together -- the case the strip is worst at.
    if (dragActive) {
        const AnimationTrack* tr = g_timeline.findTrack(dragShapeId);
        if (tr && dragKeyIndex >= 0 &&
            static_cast<size_t>(dragKeyIndex) < tr->keys.size()) {
            ImGui::SetTooltip("t = %.2f s%s", tr->keys[dragKeyIndex].time,
                              ImGui::GetIO().KeyAlt ? "  (free)" : "  (snapped)");
        }
    }
}

void Renderer::drawKeyframeSection(ShapeManager& shapeManager) {
    Shape* sel = shapeManager.getSelectedShape();
    const float contentEnd = g_timeline.duration();
    AnimationTrack* selTrack = sel ? g_timeline.findTrack(sel->getId()) : 0;

    // --- the strip ----------------------------------------------------------
    //
    // This IS the time control. There is no separate slider any more: the strip
    // shows where the playhead is and sets it, which is one surface instead of
    // two that had to be read against each other.
    drawTimelineStrip(shapeManager);

    // --- transport ----------------------------------------------------------
    //
    // Ordered outward from Play, which is the way every editing tool that has
    // one of these arranges it: coarse jumps at the ends, frame steps beside
    // Play, key jumps between them. ASCII labels rather than arrow glyphs --
    // ImGui's default font is ASCII only, and a font atlas is not worth a
    // triangle.
    const ImVec2 kSmall(30, 0), kMed(46, 0), kPlay(58, 0);

    if (ImGui::Button("|<##kf", kSmall)) {
        g_timeline.reset(shapeManager);
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Rewind to 0");

    ImGui::SameLine();
    if (ImGui::Button("<Key##kf", kMed)) {
        float t;
        g_timeline.currentTime =
            g_timeline.nearestKey(selTrack, g_timeline.currentTime, false, t) ? t : 0.0f;
        g_timeline.playing = false;
        g_timeline.apply(shapeManager);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Previous key%s", selTrack ? " on this track" : " (any track)");
    }

    ImGui::SameLine();
    if (ImGui::Button("<<##kf", kSmall)) {
        g_timeline.currentTime =
            (g_timeline.currentTime > kTimelineFrame) ? g_timeline.currentTime - kTimelineFrame
                                                      : 0.0f;
        g_timeline.playing = false;
        g_timeline.apply(shapeManager);
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Back one frame (1/30 s)");

    ImGui::SameLine();
    if (ImGui::Button(g_timeline.playing ? "Pause##kf" : "Play##kf", kPlay)) {
        g_timeline.playing = !g_timeline.playing;
    }

    ImGui::SameLine();
    if (ImGui::Button(">>##kf", kSmall)) {
        g_timeline.currentTime += kTimelineFrame;
        if (g_timeline.currentTime > g_timeline.length) g_timeline.currentTime = g_timeline.length;
        g_timeline.playing = false;
        g_timeline.apply(shapeManager);
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Forward one frame (1/30 s)");

    ImGui::SameLine();
    if (ImGui::Button("Key>##kf", kMed)) {
        float t;
        g_timeline.currentTime =
            g_timeline.nearestKey(selTrack, g_timeline.currentTime, true, t)
                ? t : g_timeline.length;
        g_timeline.playing = false;
        g_timeline.apply(shapeManager);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Next key%s", selTrack ? " on this track" : " (any track)");
    }

    ImGui::SameLine();
    if (ImGui::Button(">|##kf", kSmall)) {
        g_timeline.currentTime = g_timeline.length;
        g_timeline.playing = false;
        g_timeline.apply(shapeManager);
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Jump to the end");

    // --- numeric time and length --------------------------------------------
    //
    // Scrubbing is fast and imprecise; setting a key at exactly 2.5 s is not
    // something a 68-pixel-per-second strip can do. Both stay typeable.
    ImGui::SetNextItemWidth(96);
    if (ImGui::DragFloat("Time##kf", &g_timeline.currentTime, 0.01f,
                         0.0f, g_timeline.length, "%.2f s", kTypeable)) {
        g_timeline.playing = false;
        g_timeline.apply(shapeManager);
        sceneModified = true;
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(96);
    if (ImGui::DragFloat("Length##kf", &g_timeline.length, 0.1f,
                         0.1f, 600.0f, "%.1f s", kTypeable)) {
        // Shortening past the playhead would leave it off the end of the strip.
        if (g_timeline.currentTime > g_timeline.length) {
            g_timeline.currentTime = g_timeline.length;
        }
        sceneModified = true;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("How long the timeline runs, and where it loops.\n"
                          "Grows on its own if you key past the end.");
    }

    ImGui::Checkbox("Loop##kf", &g_timeline.loopAll);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat("Speed##kf", &g_timeline.speed, 0.01f, -4.0f, 4.0f, "%.2fx", kTypeable);

    // --- where the playhead sits relative to the selected track --------------
    //
    // The one thing the strip cannot show at this width: whether t = 1.03 is
    // between keys 2 and 3 or holding past the last one. Both look like "the
    // playhead is over there" but interpolate completely differently, and it is
    // the usual first question when a shape is not moving.
    if (selTrack && !selTrack->keys.empty()) {
        size_t before = 0;
        for (size_t i = 0; i < selTrack->keys.size(); ++i) {
            if (selTrack->keys[i].time <= g_timeline.currentTime + 1e-4f) ++before;
        }
        if (before == 0) {
            ImGui::TextDisabled("Before the first key -- holding key 1's pose");
        } else if (before >= selTrack->keys.size()) {
            ImGui::TextDisabled("Past the last key -- holding key %d's pose",
                                static_cast<int>(selTrack->keys.size()));
        } else {
            ImGui::TextDisabled("Between keys %d and %d",
                                static_cast<int>(before), static_cast<int>(before + 1));
        }
    } else {
        ImGui::TextDisabled("%d track%s, last key at %.2f s",
                            static_cast<int>(g_timeline.tracks.size()),
                            g_timeline.tracks.size() == 1 ? "" : "s", contentEnd);
    }

    ImGui::Separator();

    // --- keying the selected shape -----------------------------------------
    if (!sel) {
        ImGui::TextDisabled("Select a shape to key it.");
        return;
    }

    ImGui::Text("Selected: %s (id %d)", sel->getShapeType().c_str(), sel->getId());

    if (ImGui::Button("Set key at t", ImVec2(110, 0))) {
        g_timeline.keyShapeAt(shapeManager, sel->getId(), g_timeline.currentTime);
        sceneModified = true;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Records this shape's current position, rotation and\n"
                          "scale as a keyframe at the current time.");
    }

    AnimationTrack* tr = selTrack;

    ImGui::SameLine();
    ImGui::BeginDisabled(tr == 0);
    if (ImGui::Button("Delete key", ImVec2(110, 0))) {
        // A generous epsilon: the slider rarely lands exactly on a key, and
        // "nothing happened" is a worse answer than "removed the nearest one".
        if (tr && tr->removeKeyNear(g_timeline.currentTime, 0.05f)) {
            if (tr->keys.empty()) g_timeline.removeTrack(sel->getId());
            sceneModified = true;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear track", ImVec2(110, 0))) {
        g_timeline.removeTrack(sel->getId());
        sceneModified = true;
        dragUndoShapeId = -1;        // the snapshot refers to a track that is gone
    }
    ImGui::EndDisabled();

    // --- undoing a drag -----------------------------------------------------
    //
    // One level, and only for drags. A mis-drop is the one destructive thing
    // the strip can do without asking: land a key on another and the one
    // underneath is gone, silently, with no dialog and nothing to click. Every
    // other edit here is a button press you can see coming.
    {
        const bool canUndo = (dragUndoShapeId >= 0);
        ImGui::BeginDisabled(!canUndo);
        const bool pressed = ImGui::Button("Undo move", ImVec2(110, 0));
        ImGui::EndDisabled();

        const bool ctrlZ = canUndo && !dragActive &&
                           ImGui::GetIO().KeyCtrl &&
                           ImGui::IsKeyPressed(ImGuiKey_Z, false);

        if (canUndo && (pressed || ctrlZ)) {
            AnimationTrack* tr = g_timeline.findTrack(dragUndoShapeId);
            if (tr) {
                tr->keys = dragUndoKeys;
                g_timeline.apply(shapeManager);
                sceneModified = true;
            }
            dragUndoShapeId = -1;
        }
        if (ImGui::IsItemHovered() && canUndo) {
            ImGui::SetTooltip("Puts the last dragged track back as it was.\n"
                              "Ctrl+Z does the same.");
        }

        // Its own line rather than SameLine: after the button there is not
        // enough width left and it wrapped mid-phrase.
        ImGui::TextDisabled("Drag a key to retime  (Alt free, Esc cancel)");
    }

    if (!tr) {
        ImGui::TextDisabled("No track. Pose the shape, then Set key at t.");
        return;
    }

    ImGui::Checkbox("Loop this track", &tr->loop);

    if (ImGui::BeginTable("##keys", 5,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                          ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("t");
        ImGui::TableSetupColumn("position");
        ImGui::TableSetupColumn("rotation");
        ImGui::TableSetupColumn("scale");
        ImGui::TableSetupColumn("ease out");
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < tr->keys.size(); ++i) {
            const Keyframe& k = tr->keys[i];
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            // Clicking a row jumps the playhead to that key, which is the
            // fastest way to check what a key actually holds. Not SpanAllColumns
            // any more: the easing combo is a real widget in the last column,
            // and a Selectable spanning it would swallow the clicks.
            char label[32];
            std::snprintf(label, sizeof(label), "%.2f##k%d", k.time,
                          static_cast<int>(i));
            if (ImGui::Selectable(label)) {
                g_timeline.currentTime = k.time;
                g_timeline.playing = false;
                g_timeline.apply(shapeManager);
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2f %.2f %.2f", k.position.x, k.position.y, k.position.z);
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.0f %.0f %.0f", k.rotation.x, k.rotation.y, k.rotation.z);
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.2f %.2f %.2f", k.scale.x, k.scale.y, k.scale.z);

            ImGui::TableSetColumnIndex(4);
            // The LAST key has no interval to its right, so its easing governs
            // nothing. Showing a live combo there would invite a student to set
            // it and wonder why nothing changed.
            if (i + 1 >= tr->keys.size()) {
                ImGui::TextDisabled("--");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("A key's easing shapes the interval AFTER it.\n"
                                      "The last key has no interval after it.");
                }
            } else {
                char id[16];
                std::snprintf(id, sizeof(id), "##e%d", static_cast<int>(i));
                ImGui::SetNextItemWidth(-1);
                int mode = k.easing;
                if (ImGui::Combo(id, &mode, kEasingNames, EASE_COUNT)) {
                    tr->keys[i].easing = mode;
                    g_timeline.apply(shapeManager);
                    sceneModified = true;
                }
            }
        }
        ImGui::EndTable();
    }
}


// ---------------------------------------------------------------------------
// Left-hand panel dock
// ---------------------------------------------------------------------------

// Panels are stacked rather than each pinned to a fixed rectangle. A fixed
// full-height panel cannot share the edge with a second one, and collapsing it
// would leave a hole. Advancing a shared cursor by each window's actual height
// means a collapsed panel occupies only its title bar and everything below
// slides up to meet it.
bool Renderer::beginLeftPanel(const char* title, bool* open) {
    const float kWidth = 360.0f;
    ImGuiIO& io = ImGui::GetIO();

    float remaining = io.DisplaySize.y - leftPanelY - 4.0f;
    if (remaining < 60.0f) remaining = 60.0f;   // always leave the title usable

    ImGui::SetNextWindowPos(ImVec2(0.0f, leftPanelY), ImGuiCond_Always);
    // Fixed width, height driven by content but capped at what is left on
    // screen; the window scrolls internally beyond that.
    ImGui::SetNextWindowSizeConstraints(ImVec2(kWidth, 0.0f),
                                        ImVec2(kWidth, remaining));

    bool visible = ImGui::Begin(title, open,
                                ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_AlwaysAutoResize);

    leftPanelPushedStyles = false;
    if (visible) {
        // A fixed-width panel clips anything that assumes it can grow:
        //   - plain text runs off the right edge instead of wrapping
        //   - widgets default to ~65% of the window, leaving too little for
        //     their labels, which ImGui draws to the RIGHT of the control
        // Wrapping at the content edge and reserving label space fixes both
        // for every panel using this dock, rather than per call site.
        ImGui::PushTextWrapPos(0.0f);
        ImGui::PushItemWidth(-150.0f);   // widget = available - 150px for label
        leftPanelPushedStyles = true;
    }
    return visible;
}

void Renderer::endLeftPanel() {
    if (leftPanelPushedStyles) {
        ImGui::PopItemWidth();
        ImGui::PopTextWrapPos();
        leftPanelPushedStyles = false;
    }
    // Read the height while the window is still current, then advance.
    leftPanelY += ImGui::GetWindowHeight() + 4.0f;
    ImGui::End();
}
