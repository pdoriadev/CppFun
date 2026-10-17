#include "VRSystem.h"

#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// The canned head path. Twelve seconds, three movements, then it loops.
static const float kTrackLength = 12.0f;

VRSettings::VRSettings()
    : source(VRSystem::Source_Simulated),
      eye(VR_EYE_LEFT),
      sideBySide(false),
      worldUnitsPerMetre(1.0f),
      passthrough(false),
      simMode(VRSystem::Sim_Track),
      simPlaying(true),
      simTime(0.0f),
      simYawDeg(0.0f),
      simPitchDeg(0.0f),
      simRollDeg(0.0f),
      simX(0.0f), simY(0.0f), simZ(0.0f),
      simIpdMm(64.0f) {}

VRSystem::VRSystem()
    : activeFlag(false),
      currentPose(noHeadPose()) {
    refreshStatus();
}

float VRSystem::trackDuration() { return kTrackLength; }

// ---------------------------------------------------------------------------
// Status
// ---------------------------------------------------------------------------
// Deliberately does NOT create an OpenXR session, or an instance, or anything
// else that could wake a runtime. See the activation-policy note in the header.
// When the real source lands, the most this may do at startup is enumerate
// extensions -- which the loader answers from the runtime's manifest without
// starting it.

void VRSystem::refreshStatus() {
    if (settings.source == Source_Simulated) {
        statusText =
            "Simulated pose source. No headset, no OpenXR runtime, no loader.\n"
            "Drives the same vrViewMatrix() the headset would.";
        return;
    }

#ifdef ENABLE_VR
    statusText = "OpenXR: built in, but no runtime has been queried yet.";
#else
    statusText =
        "OpenXR support is not compiled into this build.\n"
        "Rebuild with ENABLE_VR=1 and an OpenXR loader available.\n"
        "The simulated source below needs neither and works everywhere.";
#endif
}

bool VRSystem::available(std::string& reason) const {
    if (settings.source == Source_Simulated) {
        reason.clear();
        return true;
    }

#ifdef ENABLE_VR
    // Where the real check goes: instance created, system available, session
    // reached READY. Until VRSystem's OpenXR half exists, say so plainly rather
    // than claiming a capability.
    reason = "OpenXR source is not implemented yet.";
    return false;
#else
    reason = "this build has no OpenXR support (rebuild with ENABLE_VR=1).";
    return false;
#endif
}

void VRSystem::enter() {
    std::string why;
    if (!available(why)) return;
    activeFlag = true;
}

void VRSystem::exit() {
    activeFlag = false;
}

// ---------------------------------------------------------------------------
// The simulated head
// ---------------------------------------------------------------------------
//
// Two modes. Manual is six sliders and is what you reach for when a specific
// pose is wrong and you want to sit on it. The canned path is what you reach
// for to see whether the thing moves correctly, and it is what makes a
// screen-recorded demo comparable between one submission and the next -- every
// student's video shows the same head doing the same three things.
//
// The path is generated rather than loaded from a file on purpose: a data file
// is one more asset to ship, to lose, and to have go missing from a student
// tree.

// Smooth 0->1 with zero derivative at both ends. Linear ramps read as robotic
// and, worse, hide sign errors -- a constant velocity looks the same forwards
// and backwards until it stops.
static float smoothstep01(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    return t * t * (3.0f - 2.0f * t);
}

// Segment-local time in [0,1] for a segment spanning [a,b].
static float seg(float t, float a, float b) {
    return smoothstep01((t - a) / (b - a));
}

HeadPose VRSystem::simulatedPose() const {
    HeadPose h = noHeadPose();
    h.valid = true;

    const float ipd = settings.simIpdMm * 0.001f;
    h.ipdMetres = ipd;
    h.eyeOffsetLocal[VR_EYE_LEFT]  = glm::vec3(-0.5f * ipd, 0.0f, 0.0f);
    h.eyeOffsetLocal[VR_EYE_RIGHT] = glm::vec3(+0.5f * ipd, 0.0f, 0.0f);

    float yaw, pitch, roll;
    glm::vec3 pos;

    if (settings.simMode == Sim_Manual) {
        yaw   = glm::radians(settings.simYawDeg);
        pitch = glm::radians(settings.simPitchDeg);
        roll  = glm::radians(settings.simRollDeg);
        pos   = glm::vec3(settings.simX, settings.simY, settings.simZ);
    } else {
        const float t = std::fmod(settings.simTime, kTrackLength);

        // 0-4s   look around: yaw sweeps left, right, back to centre, with a
        //        little pitch, standing still. Catches a view that does not
        //        turn, and a yaw with the wrong sign.
        // 4-8s   walk forward 1.5 m while the head returns to centre. Catches
        //        translation being dropped, or scaled by the wrong factor.
        // 8-12s  lean and crouch. Roll is the one an orbit camera cannot do at
        //        all, so it is the clearest proof the pose is really driving
        //        the view rather than being folded back into theta/phi.
        yaw = pitch = roll = 0.0f;
        pos = glm::vec3(0.0f, 0.0f, 0.0f);

        if (t < 4.0f) {
            const float u = t / 4.0f;
            yaw   = glm::radians(50.0f) * std::sin(u * 2.0f * 3.14159265f);
            pitch = glm::radians(12.0f) * std::sin(u * 4.0f * 3.14159265f);
        } else if (t < 8.0f) {
            const float u = seg(t, 4.0f, 8.0f);
            pos.z = -1.5f * u;
            yaw   = glm::radians(20.0f) * (1.0f - u);
        } else {
            const float u = (t - 8.0f) / 4.0f;
            pos.z = -1.5f;
            roll  = glm::radians(14.0f) * std::sin(u * 2.0f * 3.14159265f);
            pos.x =              0.35f  * std::sin(u * 2.0f * 3.14159265f);
            pos.y =             -0.30f  * smoothstep01(std::sin(u * 3.14159265f));
        }
    }

    // Yaw about play +Y, then pitch about the head's own X, then roll about the
    // head's own Z. Applied in that order because that is the order a neck
    // works, and because any other order makes the sliders fight each other.
    glm::mat4 r(1.0f);
    r = glm::rotate(r, yaw,   glm::vec3(0.0f, 1.0f, 0.0f));
    r = glm::rotate(r, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    r = glm::rotate(r, roll,  glm::vec3(0.0f, 0.0f, 1.0f));

    h.headToPlaySpace = glm::translate(glm::mat4(1.0f), pos) * r;

    // The runtime supplies these for real. The simulator does not pretend to
    // model an asymmetric frustum -- nothing in the assignment reads them, and
    // a plausible-looking wrong number is worse than an obvious placeholder.
    h.eyeProjection[0] = glm::mat4(1.0f);
    h.eyeProjection[1] = glm::mat4(1.0f);

    return h;
}

// ---------------------------------------------------------------------------
// Per-frame
// ---------------------------------------------------------------------------

glm::mat4 VRSystem::viewForEye(Camera& camera, VREye eye) {
    // Clear FIRST. vrViewMatrix() returns orbit.getViewMatrix() when tracking
    // is invalid, and getViewMatrix() returns the override when one is set --
    // so leaving last frame's override in place would feed the function its own
    // output, and the mouse would never get the camera back. Cheap, and the
    // caller re-applies immediately.
    camera.clearViewOverride();
    return vrViewMatrix(currentPose, eye, camera, settings.worldUnitsPerMetre);
}

void VRSystem::update(float dtSeconds, Camera& camera) {
    refreshStatus();

    if (settings.source == Source_Simulated &&
        settings.simMode == Sim_Track &&
        settings.simPlaying && activeFlag) {
        settings.simTime += dtSeconds;
        if (settings.simTime > kTrackLength) settings.simTime -= kTrackLength;
    }

    if (!activeFlag) {
        currentPose = noHeadPose();
        camera.clearViewOverride();
        return;
    }

    if (settings.source == Source_Simulated) {
        currentPose = simulatedPose();
    } else {
        // The real source would fill currentPose here. Until it exists, refuse
        // rather than invent: an invalid pose drops us back to the mouse, which
        // is the correct behaviour for lost tracking anyway.
        currentPose = noHeadPose();
    }

    if (!currentPose.valid) {
        // Automatic exit. Losing tracking must not leave a frozen viewport --
        // hand the camera back on the same frame it happens.
        activeFlag = false;
        camera.clearViewOverride();
        return;
    }

    const VREye primary =
        (settings.eye == VR_EYE_RIGHT) ? VR_EYE_RIGHT : VR_EYE_LEFT;
    camera.setViewOverride(viewForEye(camera, primary));
}
