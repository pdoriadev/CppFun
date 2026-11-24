#ifndef VRSYSTEM_H
#define VRSYSTEM_H

#include <string>

#include "HeadPose.h"

class Camera;

// =============================================================================
// VRSystem -- the given side of the A3 extra credit
// =============================================================================
//
// Owns one job: produce a HeadPose each frame, and hand it to the student's
// vrViewMatrix() to get a view matrix the renderer can use. Everything the
// panel shows and every knob it offers lives in here.
//
// -----------------------------------------------------------------------------
// TWO POSE SOURCES
// -----------------------------------------------------------------------------
//
//   Source_Simulated   sliders and a canned head path. No OpenXR, no loader,
//                      no headset, no hardware of any kind. Works in every
//                      build on every platform in the class.
//
//   Source_OpenXR      the real thing. Compiled only when ENABLE_VR is
//                      defined; absent otherwise, and the panel says so
//                      rather than hiding the option.
//
// The simulated source is not a stand-in for testing that gets deleted later.
// It is a first-class feature and the reason this extra credit is assignable
// at all: a class of thirty does not have thirty headsets. A student can do
// the work, see it move, and demonstrate it; a grader can mark it without
// booking hardware. The headset makes it immersive, which is a different thing
// from making it correct.
//
// -----------------------------------------------------------------------------
// ACTIVATION POLICY -- deliberate, do not "improve" this into auto-start
// -----------------------------------------------------------------------------
//
// Detection decides whether VR is AVAILABLE. The user decides whether it is ON.
// Never the other way round, for four reasons:
//
//   * finding out whether a headset is really there is not a passive query --
//     it means creating a session, which wakes the runtime service and can
//     pull Link or SteamVR onto the screen. Every student, every launch, on
//     machines with no headset.
//
//   * "detected" is not one state. A live runtime, an available system and a
//     session that has reached FOCUSED are three different things, and a Quest
//     asleep on a desk can still report a system.
//
//   * the moment the override is set the mouse stops steering the viewport. A
//     student who plugged their headset in to charge would launch the editor,
//     find that orbit and pan do nothing, and conclude their CameraControl.cpp
//     is broken. That is exactly the confusion the seam exists to prevent.
//
//   * grading stops reproducing. The same submission on two machines would
//     produce two different views and the screenshots in the deliverables
//     would not match.
//
// EXIT is automatic, and that asymmetry is on purpose: entering is a choice,
// leaving is a safety net. Lose tracking, unplug the headset or take it off and
// the override is dropped and the mouse takes over on the next frame.
//
// The one concession to convenience is AppSettings::autoEnterVR, off by
// default. It only ever fires for a real device source, so ticking it on a
// development machine costs a student nothing.

struct VRSettings {
    int  source;                 // VRSystem::Source_*
    int  eye;                    // VR_EYE_LEFT / VR_EYE_RIGHT -- which eye
                                 // drives the desktop window
    bool sideBySide;             // draw both eyes, split screen

    float worldUnitsPerMetre;    // play space is in metres; the scene is not
    bool  passthrough;           // requested; the simulated source ignores it

    // --- simulated source -------------------------------------------------
    int   simMode;               // VRSystem::Sim_*
    bool  simPlaying;
    float simTime;               // seconds into the canned path
    float simYawDeg;             // manual mode
    float simPitchDeg;
    float simRollDeg;
    float simX, simY, simZ;      // metres, play space
    float simIpdMm;              // interpupillary distance

    VRSettings();
};

class VRSystem {
public:
    enum Source  { Source_Simulated = 0, Source_OpenXR = 1 };
    enum SimMode { Sim_Manual = 0, Sim_Track = 1 };

    VRSystem();

    VRSettings settings;

    // Called once per frame from renderScene(), BEFORE the view matrix is read.
    //
    // Advances the simulated clock, rebuilds the pose, and applies or clears
    // the camera's view override. When it clears, the mouse is back in charge
    // on this very frame -- there is no lag between losing tracking and getting
    // the orbit camera back.
    void update(float dtSeconds, Camera& camera);

    // The view matrix for one eye. Used by the side-by-side path, which needs
    // both within a single frame.
    //
    // Clears the override before calling the student's function, because
    // vrViewMatrix() reads orbit.getViewMatrix() on its no-tracking path -- and
    // if the override were still set from the previous frame it would read back
    // its own output and the mouse would never regain control.
    glm::mat4 viewForEye(Camera& camera, VREye eye);

    bool active() const { return activeFlag; }
    void enter();
    void exit();

    // Can the currently selected source be entered right now? When false,
    // `reason` is filled in with something a person can act on.
    bool available(std::string& reason) const;

    // Is this a real device, as opposed to the simulator? Only a real device
    // source honours AppSettings::autoEnterVR.
    bool sourceIsDevice() const { return settings.source == Source_OpenXR; }

    // What the panel prints at the top: runtime, binding, headset.
    const std::string& status() const { return statusText; }

    // This frame's pose, for the panel's readout. The readout is what lets a
    // student tell "my matrix is wrong" from "my headset is not tracking",
    // which are otherwise the same symptom.
    const HeadPose& pose() const { return currentPose; }

    // Length of the canned path, seconds.
    static float trackDuration();

private:
    bool        activeFlag;
    HeadPose    currentPose;
    std::string statusText;

    void      refreshStatus();
    HeadPose  simulatedPose() const;
};

#endif // VRSYSTEM_H
