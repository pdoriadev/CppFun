#ifndef RENDERER_H
#define RENDERER_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>
#include <algorithm>  // For std::find

#include "Globals.h"
#include "ShapeManager.h"
#include "TimeStepper.h"
#include "Shape.h"
#include "Cube.h"
#include "Custom.h"
#include "ImportShape.h"
#include "Icosahedron.h" 
#include "Pyramid.h"
#include "Sphere.h"
#include "Teapot.h"
#include "SimpleSystem.h"
#include "SimplePendulum.h"
#include "SimpleChain.h"
#include "SimpleCloth.h"
#include "FileImporter.h"

#include "Keyframe.h"
#include "Camera.h"
#include "RayTracer.h"
#include "RayScene.h"
#include "VRSystem.h"
#include <string>

class Renderer {
public:
    Renderer();

    // Shader utilities
    GLuint getShaderProgram() const;
    void setShaderProgram(GLuint shader);

    // Public methods for controlling the rendering pipeline
    void setupLighting(GLuint shaderProg);
    void uploadLights(GLuint shaderProg, ShapeManager& shapeManager);
    void drawAxis(GLuint shaderProgram);
    void renderScene(ShapeManager& shapeManager, TimeStepper* timeStepper);

    const Camera& getCamera() const { return camera; }


private:
    // Camera state now lives in a shared Camera so the ray tracer and the
    // picker can generate rays that match what the rasterizer draws.
    Camera camera;

    // --- ray casting / ray tracing ---
    RayTracer rayTracer;
    RayTraceSettings traceSettings;
    bool showTracePanel;
    bool overlayPreview;      // draw the traced image over the 3D viewport
    bool showLightGizmos;   // View menu: hide light markers
    VRSystem vrSystem;
    bool showVRPanel;
    std::string lastSaveMessage;
    std::string optionsSaveMessage;

    // Click-to-select. Returns true if a shape was hit.
    bool pickShape(ShapeManager& shapeManager, float mouseX, float mouseY,
                   int viewportWidth, int viewportHeight);

    // --- scene file state ---
    std::string currentScenePath;   // empty until saved or opened
    bool        sceneModified;

    // Dirty tracking by content signature rather than a flag sprinkled across
    // every mutation site: shapes can be edited from many places (menus,
    // property sliders, importers) and any missed call site silently loses the
    // "unsaved" state. Hashing what would actually be written cannot drift.
    unsigned long long savedSignature;
    unsigned long long sceneSignature(ShapeManager& shapeManager) const;

    // The startup scene is built by Application AFTER this object is
    // constructed, so the constructor cannot take its baseline signature - it
    // would hash an empty shape list and every later frame would compare
    // against it and report unsaved changes. Instead the first frame that sees
    // a scene takes the baseline and sets this.
    bool signatureBaselined;
    std::string sceneStatusMessage;
    bool        confirmDiscardOpen; // modal: New/Open with unsaved work
    int         pendingSceneAction; // 0 none, 1 New, 2 Open
    std::string pendingScenePath;   // for Open Recent

    // Left-hand dock. Panels stack downward from the menu bar; each advances
    // the cursor by its own height, so collapsing one lifts the others up.
    float leftPanelY;
    bool  leftPanelPushedStyles;
    bool  beginLeftPanel(const char* title, bool* open);
    void  endLeftPanel();

    bool showAnimationPanel;
    void drawAnimationMenu(ShapeManager& shapeManager);
    void drawAnimationPanel(ShapeManager& shapeManager);
    // The animation panel's two halves. They are separate systems -- particle
    // systems integrate forwards and cannot be scrubbed, keyframe tracks are a
    // pure function of t -- and separate assignments, so they get separate
    // methods and separate named headers rather than two identical transports.
    void drawParticleSection(ShapeManager& shapeManager);

    // The keyframe timeline: a ruler, one row per track, keys as diamonds and
    // a draggable playhead, drawn by hand into the window's draw list. It is
    // the time CONTROL as well as the display -- it replaced the slider that
    // used to sit under the transport.
    void drawTimelineStrip(ShapeManager& shapeManager);
    void drawKeyframeSection(ShapeManager& shapeManager);

    // --- keyframe drag state ------------------------------------------------
    //
    // A drag is tracked by (shapeId, index) rather than a Keyframe* because the
    // vector reorders as the key passes its neighbours -- AnimationTrack::moveKey
    // returns the new index each frame and this follows it.
    //
    // `dragArmed` is the press; `dragActive` is the press once it has travelled
    // far enough to be a drag rather than a click. Everything between those two
    // states still scrubs, so a plain click on a diamond behaves as it did
    // before this existed.
    int   dragShapeId;        // -1 when nothing is grabbed
    int   dragKeyIndex;
    float dragStartTime;      // the key's time when the press began, for Esc
    float dragPressX;         // screen x of the press, for the threshold
    bool  dragArmed;
    bool  dragActive;

    // One level of undo, captured at the moment a drag turns real. A mis-drop
    // is silent and destructive -- it can eat a key by landing on it -- so the
    // whole track is snapshotted rather than just the one time value.
    int                   dragUndoShapeId;
    std::vector<Keyframe> dragUndoKeys;

    void drawFileMenu(ShapeManager& shapeManager);
    void drawSceneModals(ShapeManager& shapeManager);
    void newScene(ShapeManager& shapeManager);
    void openScene(ShapeManager& shapeManager, const std::string& path);
    bool saveScene(ShapeManager& shapeManager, const std::string& path);
    void promptSaveAs(ShapeManager& shapeManager);
    void promptOpen(ShapeManager& shapeManager);
    void updateWindowTitle();

    bool showOptionsWindow;
    void drawOptionsWindow();
    void drawSelectedObjectProperties(ShapeManager& shapeManager);

    void drawRayTracePanel(ShapeManager& shapeManager);
    void drawVRPanel();
    void drawPreviewOverlay();

    float translateX, translateY, translateZ;
    float angleX, angleY;
    float scaleFactor;
    
    // Boolean variable to track axis visibility
    bool showAxis = true;

    IntegratorType selectedIntegrator;

    GLuint shaderProgram; // Holds the active shader program

};

#endif  // RENDERER_H
