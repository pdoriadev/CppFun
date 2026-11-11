// =============================================================================
// vrViewMatrix -- turn a tracked head pose into a view matrix
// =============================================================================
// Assignment 3, EXTRA CREDIT, in both courses. Topic: `vrcamera`.
//
// One function, in a file of its own, for the same reason CameraControl.cpp is
// a file of its own: a translation unit is the smallest thing a solution
// archive can swap.
//
// -----------------------------------------------------------------------------
// WHERE THIS SITS
// -----------------------------------------------------------------------------
// Everything about OpenXR -- creating the session, negotiating a graphics
// binding, waiting on frames, locating the views, the swapchains, passthrough
// -- is GIVEN, in VRSystem.cpp, and you are not expected to read it. It exists
// so that by the time control reaches this file the tracking has already been
// reduced to a HeadPose: a matrix, two offsets, two projections. Read
// include/HeadPose.h; the comments in it are the contract.
//
// What is left is a graphics problem and nothing else: you have a chain of
// transforms and you need the matrix that takes world coordinates into the
// eye's frame.
//
// It is also why this compiles and is graded on a machine with no headset
// attached. ./vr_view_test fabricates poses. You do not need a Quest to do
// this work, or to know whether you have done it right -- you need one to see
// it, which is a different thing and is what the demo is for.
//
// -----------------------------------------------------------------------------
// NO QUATERNIONS
// -----------------------------------------------------------------------------
// The runtime reports orientation as a quaternion, because that is what
// tracking systems do. The given code converts it to a rotation matrix before
// it reaches you, so `headToPlaySpace` is an ordinary rigid transform of the
// kind you have been composing since Assignment 2. You are welcome to go and
// read about quaternions -- they are how this is done in practice, and they
// are the right tool for interpolating orientation -- but nothing here needs
// them and this course does not assume them.

#include "HeadPose.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 vrViewMatrix(const HeadPose& head,
                       VREye eye,
                       const Camera& orbit,
                       float worldUnitsPerMetre) {
    // TODO(vrcamera): build eye-to-world by composing play placement with the head pose, then invert it
    // A view matrix is world-to-camera. The easy way to build one is to
    // build camera-to-world first -- because that direction is just the
    // chain of places the eye lives inside -- and then invert it.
    //
    // The chain, outermost first:
    //
    //     world  <--  play space  <--  head  <--  eye
    //
    //   1. Bail out first. If `head.valid` is false there is no tracking
    //      this frame, and the honest answer is orbit.getViewMatrix() --
    //      the mouse keeps working and the viewport does not go black
    //      because someone set the headset down.
    //
    //   2. play -> world. The mouse decides this, and it is the reason
    //      this is a composition and not just "use the headset instead of
    //      the camera". Translate by orbit.getTarget() -- the point the
    //      orbit camera is looking at is where you put the room -- and
    //      scale by `worldUnitsPerMetre`, because play space is in real
    //      metres and the scene is in units nobody ever defined.
    //      Guard the scale: a zero makes the matrix singular and inverting
    //      it gives you a viewport full of NaN.
    //
    //   3. head -> play is handed to you: head.headToPlaySpace.
    //
    //   4. eye -> head is a translation by head.eyeOffsetLocal[eye].
    //      Which SIDE it goes on matters. The offset is expressed in head
    //      space, so it must be applied before the head transform --
    //      i.e. on the RIGHT: headToPlaySpace * translate(offset). Put it
    //      on the left and the eyes stay pinned to the room's X axis while
    //      the head turns, which looks almost right sitting still and is
    //      unbearable the moment you move.
    //
    //   5. Multiply the chain into eye-to-world, and return its inverse.
    //
    // What "wrong" looks like, so you can tell the cases apart:
    //   - view frozen while the panel's pose readout moves ..... this
    //     function is still the stub, or you returned the orbit matrix
    //     unconditionally
    //   - the world swims in the opposite direction to your head ....
    //     you returned eye-to-world instead of its inverse
    //   - everything doubled or with a headache-shaped depth ......
    //     the two eyes are swapped, or you ignored the eye offset
    //   - the scene is inside your face ....... worldUnitsPerMetre
    //
    // ./vr_view_test checks all four of those without a headset.
    (void)head; (void)eye; (void)worldUnitsPerMetre;
    return orbit.getViewMatrix();
}
