// =============================================================================
// Camera::orbit / pan / zoom -- the three mouse-driven camera controls
// =============================================================================
// Three member functions, in a file of their own. They are still members of
// Camera; only the definitions live here. Camera.h declares them, nothing about
// the class changed, and Renderer's mouse handling is unaffected -- the linker
// simply finds the symbols in CameraControl.o instead of Camera.o.
//
// The point of the split is that a translation unit is the smallest thing a
// solution archive can swap. Camera.cpp holds the constructor, updatePosition(),
// the view and projection matrices and generateRay() -- all given, none of it
// the student's work -- so it cannot be swapped as a unit. This can. Same
// surgery ShapeNormals.cpp needed for `geometry` and RayIntersect.cpp for `bvh`.
// See SOLUTION-LIBRARIES.md and ASSIGNMENTS.md.
//
// A3: Mesh Loading and Camera Control, in both courses.
//
// -----------------------------------------------------------------------------
// WHAT STAYS GIVEN, AND WHY IT MATTERS
// -----------------------------------------------------------------------------
// updatePosition() is NOT here. It is the one line of spherical-to-Cartesian
// arithmetic that turns (theta, phi, radius) into an eye point, and it is called
// by the constructor, by setView(), by setTarget() and by setRadius(). If it
// were stubbed, the camera would sit at the origin looking at itself, lookAt()
// would produce a NaN basis, and every scene in every assignment would render
// black -- including Assignment 2, which has nothing to do with camera control.
//
// So the arrangement is: the STATE is given and always correct, and the three
// functions that CHANGE that state are the assignment. Stub them and the camera
// is simply frozen at its default -- on +Z, five units out, looking at the
// origin. That is exactly the view A2 is written against, which is why A2 needs
// no code change of its own when this topic ships.

#include "Camera.h"

#include <glm/gtc/constants.hpp>
#include <cmath>

// -----------------------------------------------------------------------------
// orbit -- right-drag. Swing the eye around the target.
// -----------------------------------------------------------------------------
void Camera::orbit(float dTheta, float dPhi) {
    // TODO(camera): add the deltas to theta and phi, clamp the pitch, then updatePosition()
    // The camera's eye point is derived from three numbers held by this
    // class: `theta` (yaw), `phi` (pitch) and `radius` (distance from the
    // target). Orbiting means changing the two angles and then rebuilding
    // the eye point from them.
    //
    //   1. add dTheta to theta and dPhi to phi
    //   2. CLAMP phi. Straight up is phi = +pi/2, and there the view
    //      direction becomes parallel to the up vector (0, 1, 0). lookAt()
    //      crosses those two to build its basis, a cross product of parallel
    //      vectors is zero, and normalising zero is NaN -- the whole screen
    //      goes black or white and stays that way. Stop a little short of
    //      both poles: glm::half_pi<float>() - 0.1f is plenty.
    //   3. call updatePosition(), which is given. Skipping this is the
    //      classic symptom: the angles change, the camera does not move.
    //
    // theta is deliberately NOT clamped or wrapped -- yaw is periodic, and
    // cos/sin handle a theta of 400 radians perfectly well.
    (void)dTheta; (void)dPhi;
}

// -----------------------------------------------------------------------------
// pan -- left-drag. Slide the target (and with it the eye) across the screen.
// -----------------------------------------------------------------------------
void Camera::pan(float dxScreen, float dyScreen) {
    // TODO(camera): move the target along the camera's own right and up axes, then updatePosition()
    // Panning moves what the camera is looking AT. The eye follows for free,
    // because updatePosition() derives it from the target.
    //
    // The deltas arrive in SCREEN space -- x to the right of the window, y
    // down it -- so they have to be turned into world-space directions
    // before they mean anything. The camera's own basis is what does that:
    //
    //   forward = target - position
    //   right   = normalize(cross(forward, up))
    //
    // Then move `target` along `right` and along `up` by the two deltas, and
    // call updatePosition().
    //
    // Two things to get right:
    //   - the SIGNS. Dragging left should carry the scene left, which means
    //     the target moves right. Get one backwards and the camera fights
    //     the mouse; both, and it feels fine until you try to orbit.
    //   - guard the degenerate case. If the eye ever reaches the target,
    //     `forward` is the zero vector and normalizing the cross product is
    //     NaN. Bail out early when its length is tiny.
    (void)dxScreen; (void)dyScreen;
}

// -----------------------------------------------------------------------------
// zoom -- scroll wheel. Move the eye in and out along the view direction.
// -----------------------------------------------------------------------------
void Camera::zoom(float amount) {
    // TODO(camera): shrink the radius, keep it above a floor, then updatePosition()
    // This is a dolly, not a zoom: the field of view is untouched and the
    // eye actually moves. Scrolling forward gives a POSITIVE `amount` and
    // should bring the camera CLOSER, so the radius goes down as the amount
    // goes up.
    //
    // Clamp the radius to a floor -- 0.5 is what the rest of this class
    // assumes. Without it, scrolling far enough puts the eye exactly on the
    // target (forward becomes zero, the basis is NaN) and then straight
    // through it, at which point the scene is inside out and every control
    // is mirrored.
    //
    // Then call updatePosition().
    (void)amount;
}
