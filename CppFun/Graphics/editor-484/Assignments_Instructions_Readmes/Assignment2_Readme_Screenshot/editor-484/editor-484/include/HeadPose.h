#ifndef HEADPOSE_H
#define HEADPOSE_H

#include <glm/glm.hpp>

// =============================================================================
// HeadPose -- one frame's worth of tracking, in types the student already knows
// =============================================================================
//
// This is the boundary between the OpenXR plumbing (given, VRSystem.cpp) and
// the assignment (yours, VRCamera.cpp). Nothing in this header mentions
// OpenXR, and VRCamera.cpp does not include an OpenXR header, on purpose:
//
//   * the extra credit is a GRAPHICS problem -- compose some transforms, then
//     invert -- and it should not be gated on getting an SDK installed;
//   * VRCamera.cpp has to compile on every machine in the class, including the
//     Macs, where there is no runtime to talk to at all;
//   * a grader can fabricate a HeadPose. It cannot fabricate an XrSession. So
//     this work can be tested headless, by ./vr_view_test, with no headset in
//     the room. That is the whole reason the struct looks like this.
//
// The runtime actually hands out a quaternion. The conversion to a matrix
// happens in the given code, above this line, so nothing below it needs
// quaternions -- see the note in VRCamera.cpp.
//
// -----------------------------------------------------------------------------
// THE THREE SPACES
// -----------------------------------------------------------------------------
//
//   HEAD space   origin between the eyes, -Z out the front of the face,
//                +X right, +Y up. Each eye sits at a fixed offset in it.
//
//   PLAY space   the room. Its origin is wherever the user last recentred,
//                normally the floor under where they were standing. Y is up.
//                MEASURED IN METRES -- this is the one place in the whole
//                editor with real-world units in it, and the reason
//                `worldUnitsPerMetre` exists.
//
//   WORLD space  the scene. A cube inserted from the menu is 1 unit across
//                and nobody ever said whether that unit is a metre or a
//                kilometre. Placing play space inside world space -- where the
//                room sits in the scene, which way it faces, and how big a
//                metre is -- is the part the mouse still controls.
//
// The tracking system fills in head-relative-to-play. You supply
// play-relative-to-world. Those two together are the view matrix.

struct HeadPose {
    // False when tracking has not started, has been lost, or the headset was
    // taken off. Every other field is meaningless when this is false, and the
    // panel falls back to the mouse. Check it first.
    bool valid;

    // Head-to-play-space: rotation and translation together, metres.
    // Column 3 is where the head is; the upper-left 3x3 is which way it faces.
    // Orthonormal, no scale -- a head cannot stretch.
    glm::mat4 headToPlaySpace;

    // Where each eye sits relative to the head origin, in HEAD space, metres.
    // Index 0 is the left eye, 1 is the right. Roughly (-0.032, 0, 0) and
    // (+0.032, 0, 0) for a 64 mm interpupillary distance, but the runtime
    // reports the user's own, and it is not necessarily symmetric.
    glm::vec3 eyeOffsetLocal[2];

    // The projection for each eye, supplied by the runtime and NOT a
    // glm::perspective. A headset's frustum is asymmetric -- you see further
    // towards your temple than towards your nose -- so the four half-angles
    // differ and there is no single fov that describes it. Given, use as-is.
    glm::mat4 eyeProjection[2];

    // Interpupillary distance in metres, for display. Derived from the two
    // offsets; kept here so the panel does not have to recompute it.
    float ipdMetres;
};

// Index into the two-element arrays above. Named because `pose.eyeOffsetLocal[1]`
// at a call site is a coin flip, and getting it backwards produces a picture
// that looks correct and makes people ill.
enum VREye { VR_EYE_LEFT = 0, VR_EYE_RIGHT = 1 };

// A pose that says "no tracking". Handy default, and what the panel uses when
// the headset is off, so nothing downstream has to special-case a null.
HeadPose noHeadPose();

class Camera;

// -----------------------------------------------------------------------------
// The extra credit. Implemented in src/VRCamera.cpp.
// -----------------------------------------------------------------------------
//
// Turn one eye's tracked pose into a world-to-camera matrix that the renderer
// can hand straight to Camera::setViewOverride().
//
//   head                 this frame's tracking, above
//   eye                  VR_EYE_LEFT or VR_EYE_RIGHT
//   orbit                the mouse-driven camera, which decides where play
//                        space sits in the scene -- read from it, do not
//                        modify it
//   worldUnitsPerMetre   scale factor, from the VR panel slider. 1.0 means a
//                        metre of room is a unit of scene.
//
// Return a world-to-camera matrix. When `head.valid` is false, return
// `orbit.getViewMatrix()` -- the mouse keeps working and the viewport does not
// go black because someone put the headset down.
glm::mat4 vrViewMatrix(const HeadPose& head,
                       VREye eye,
                       const Camera& orbit,
                       float worldUnitsPerMetre);

#endif // HEADPOSE_H
