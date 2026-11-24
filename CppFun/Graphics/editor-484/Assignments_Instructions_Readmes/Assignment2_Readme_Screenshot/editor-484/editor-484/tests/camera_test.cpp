// Grader for the `camera` topic: orbit, pan and zoom.
//
// A3, both courses, alongside obj_parser_test.
//
// Headless and GL-free. The Camera class is pure arithmetic over three numbers
// -- theta, phi and radius -- so nothing here needs a context, a window or a
// scene. It links against exactly two objects: Camera.o (given) and
// CameraControl.o (the assignment).
//
// PROPERTY-BASED, MOSTLY. There is no expected eye position anywhere in this
// file, because a student may reasonably scale their deltas differently or
// clamp the pitch a little tighter than the reference does. What is checked
// instead is the set of things that must be true of ANY working orbit camera:
//
//   * the eye stays exactly `radius` from the target, always
//   * the pitch never reaches the poles, however hard it is pushed
//   * the radius never reaches zero, however far it is scrolled
//   * nothing ever goes NaN
//   * the state actually reaches the eye point -- updatePosition() was called
//
// The one place direction IS asserted is the sign of each control, because the
// deltas arrive straight from the mouse (see Renderer.cpp, the three call sites
// around line 930) and a camera that moves the wrong way when you drag it is
// wrong, not merely different. Each of those messages says which way is which.
//
// THE NaN CHECKS ARE THE POINT. Both failures this suite is really hunting --
// pitch reaching the pole, radius reaching the target -- produce a cross
// product of parallel vectors inside lookAt(), then a division by zero, then a
// view matrix full of NaN. The symptom is a screen that goes black or white and
// never comes back, with nothing in it to suggest the camera. A student who has
// not clamped will find out here instead of at 2am.

#include <cstdio>
#include <cmath>
#include <string>

#include <glm/glm.hpp>

#include "Camera.h"

static int checks = 0, failures = 0;

static void ok(bool cond, const std::string& what) {
    ++checks;
    if (cond) std::printf("  PASS  %s\n", what.c_str());
    else    { std::printf("  FAIL  %s\n", what.c_str()); ++failures; }
}

static void nearly(float a, float b, const std::string& what, float eps = 1e-3f) {
    ++checks;
    if (std::fabs(a - b) <= eps) {
        std::printf("  PASS  %s\n", what.c_str());
    } else {
        std::printf("  FAIL  %s\n"
                    "         got      %.6f\n"
                    "         expected %.6f\n", what.c_str(), a, b);
        ++failures;
    }
}

static bool finite3(const glm::vec3& v) {
    return v.x == v.x && v.y == v.y && v.z == v.z;
}

// Every entry of the 4x4 is a real number. This is what actually breaks when
// the pitch or the radius is left unclamped.
static bool finiteView(const Camera& c) {
    const glm::mat4 m = c.getViewMatrix();
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            if (m[col][row] != m[col][row]) return false;
        }
    }
    return true;
}

// The invariant that ties the three state variables to the eye point. It holds
// after every operation on a correct camera, and it is broken the moment
// updatePosition() is skipped after a change to the state.
static float radiusError(const Camera& c) {
    return std::fabs(glm::length(c.getPosition() - c.getTarget()) - c.getRadius());
}

int main() {
    std::printf("Camera controls\n");

    const float PI = 3.14159265358979f;

    // --- the starting point --------------------------------------------------
    //
    // Given code, so this is a tripwire rather than a grade -- but it is the
    // view Assignment 2 is written against, and every check below measures from
    // it, so it is worth pinning down before anything moves.
    {
        Camera c;
        nearly(c.getRadius(), 5.0f, "a fresh camera sits 5 units from the target");
        ok(glm::length(c.getTarget()) < 1e-5f,
           "a fresh camera looks at the world origin");
        ok(c.getPosition().z > 4.9f && std::fabs(c.getPosition().x) < 1e-3f
                                    && std::fabs(c.getPosition().y) < 1e-3f,
           "a fresh camera sits on +Z looking down -Z -- the fixed view "
           "Assignment 2 is composed against");
        nearly(radiusError(c), 0.0f,
               "the eye is exactly `radius` from the target to begin with");
        ok(finiteView(c), "the initial view matrix is finite");
    }

    // --- orbit ---------------------------------------------------------------
    {
        Camera c;
        const glm::vec3 before = c.getPosition();
        c.orbit(0.0f, 0.0f);
        ok(glm::length(c.getPosition() - before) < 1e-5f,
           "orbit: a zero drag does not move the camera");
    }
    {
        Camera c;
        const float theta0 = c.getTheta();
        const glm::vec3 before = c.getPosition();

        c.orbit(0.4f, 0.0f);

        nearly(c.getTheta(), theta0 + 0.4f,
               "orbit: the yaw delta is ADDED to theta");
        ok(glm::length(c.getPosition() - before) > 0.1f,
           "orbit: the eye actually moves -- changing theta without calling "
           "updatePosition() leaves the camera exactly where it was");
        nearly(radiusError(c), 0.0f,
               "orbit: the eye stays exactly `radius` from the target -- "
               "orbiting is a rotation, not a dolly");
        ok(glm::length(c.getTarget()) < 1e-5f,
           "orbit: the target does not move; only the eye swings around it");
    }
    {
        Camera c;
        const float phi0 = c.getPhi();
        c.orbit(0.0f, 0.3f);
        nearly(c.getPhi(), phi0 + 0.3f,
               "orbit: a positive pitch delta raises the camera");
        ok(c.getPosition().y > 0.5f,
           "orbit: raising the pitch puts the eye ABOVE the target, not below "
           "-- +phi is up");
        nearly(radiusError(c), 0.0f,
               "orbit: pitching keeps the eye at the same distance");
    }

    // The clamp. This is the check that matters.
    {
        Camera c;
        for (int i = 0; i < 200; ++i) c.orbit(0.0f, 0.5f);   // straight up, hard

        ok(std::fabs(c.getPhi()) < PI * 0.5f,
           "orbit: pitch is CLAMPED short of straight up -- at phi = +pi/2 the "
           "view direction is parallel to the up vector and lookAt() divides "
           "by zero");
        ok(finite3(c.getPosition()), "orbit: the eye is finite at the top of the arc");
        ok(finiteView(c),
           "orbit: the view matrix has no NaN at the top of the arc -- this is "
           "the black screen that never comes back");
        nearly(radiusError(c), 0.0f,
               "orbit: the distance survives being pushed against the clamp");
        ok(c.getPosition().y > 0.0f,
           "orbit: after driving the pitch up, the eye is above the target");
    }
    {
        Camera c;
        for (int i = 0; i < 200; ++i) c.orbit(0.0f, -0.5f);  // straight down

        ok(std::fabs(c.getPhi()) < PI * 0.5f,
           "orbit: pitch is clamped short of straight DOWN as well -- both "
           "poles break, not just the top one");
        ok(finiteView(c), "orbit: the view matrix has no NaN at the bottom of the arc");
        ok(c.getPosition().y < 0.0f,
           "orbit: after driving the pitch down, the eye is below the target");
    }
    {
        // Yaw is periodic and must NOT be clamped: spinning round and round is
        // a normal thing to do with a mouse, and a camera that jams after two
        // turns is broken in a way nobody thinks to test.
        Camera c;
        const glm::vec3 start = c.getPosition();
        for (int i = 0; i < 100; ++i) c.orbit(2.0f * PI / 100.0f, 0.0f);

        ok(glm::length(c.getPosition() - start) < 1e-2f,
           "orbit: a full 2pi of yaw comes back to where it started -- theta "
           "wraps naturally through cos/sin and must not be clamped");
        ok(finiteView(c), "orbit: the view matrix survives a full revolution");
    }

    // --- zoom ----------------------------------------------------------------
    {
        Camera c;
        const float r0 = c.getRadius();
        c.zoom(1.0f);
        ok(c.getRadius() < r0,
           "zoom: scrolling FORWARD (a positive amount) moves the camera "
           "CLOSER -- the radius goes down as the amount goes up");
        nearly(radiusError(c), 0.0f,
               "zoom: the eye follows the radius -- changing `radius` without "
               "calling updatePosition() leaves the camera where it was");
    }
    {
        Camera c;
        const float r0 = c.getRadius();
        c.zoom(-2.0f);
        ok(c.getRadius() > r0, "zoom: a negative amount pulls the camera back");
        nearly(radiusError(c), 0.0f, "zoom: the eye follows the radius outwards too");
    }
    {
        // Zooming is a dolly along the view ray: the direction from target to
        // eye is unchanged, only the distance.
        Camera c;
        c.orbit(0.7f, 0.4f);
        const glm::vec3 dir0 = glm::normalize(c.getPosition() - c.getTarget());
        c.zoom(2.0f);
        const glm::vec3 dir1 = glm::normalize(c.getPosition() - c.getTarget());
        nearly(glm::dot(dir0, dir1), 1.0f,
               "zoom: the eye moves straight along the view ray -- the "
               "direction from the target is unchanged", 1e-4f);
    }
    {
        // The floor. Scroll far past the target and see what happens.
        Camera c;
        c.zoom(1000.0f);

        ok(c.getRadius() > 0.0f,
           "zoom: the radius never reaches zero -- at zero the eye is ON the "
           "target and lookAt() has no direction to build a basis from");
        ok(finite3(c.getPosition()), "zoom: the eye is finite after a huge scroll");
        ok(finiteView(c),
           "zoom: the view matrix has no NaN after a huge scroll");

        // And it must not have gone through and out the other side, which is
        // what an unclamped `radius -= amount` does: the scene turns inside out
        // and every other control starts working backwards.
        ok(c.getPosition().z > 0.0f,
           "zoom: the eye stops in FRONT of the target -- a negative radius "
           "flips the camera through to the far side and mirrors everything");

        // Scrolling again while pinned must be harmless.
        const float pinned = c.getRadius();
        c.zoom(1000.0f);
        nearly(c.getRadius(), pinned,
               "zoom: scrolling again while pinned at the floor changes nothing");
    }

    // --- pan -----------------------------------------------------------------
    {
        Camera c;
        const glm::vec3 t0 = c.getTarget();
        c.pan(0.0f, 0.0f);
        ok(glm::length(c.getTarget() - t0) < 1e-5f,
           "pan: a zero drag does not move the target");
    }
    {
        // From the default view -- eye on +Z, up = +Y -- the camera's own right
        // axis is world +X, so a horizontal pan has to move the target along X
        // and nothing else.
        Camera c;
        c.pan(0.5f, 0.0f);

        ok(std::fabs(c.getTarget().y) < 1e-4f && std::fabs(c.getTarget().z) < 1e-4f,
           "pan: a horizontal drag moves the target along the camera's RIGHT "
           "axis only -- screen x has to be rotated into world space, not used "
           "as a world axis");
        nearly(glm::length(c.getTarget()), 0.5f,
               "pan: the target moves by the size of the drag");
        ok(c.getTarget().x < 0.0f,
           "pan: dragging right carries the SCENE right, which means the "
           "target moves left (target -= right * dx). Reversed, the camera "
           "fights the mouse");
        nearly(radiusError(c), 0.0f,
               "pan: the eye follows the target -- panning slides the whole "
               "camera sideways, it does not swing the eye around a new target");
    }
    {
        Camera c;
        c.pan(0.0f, 0.5f);
        ok(std::fabs(c.getTarget().x) < 1e-4f && std::fabs(c.getTarget().z) < 1e-4f,
           "pan: a vertical drag moves the target along the camera's UP axis only");
        ok(c.getTarget().y > 0.0f,
           "pan: a positive vertical delta raises the target (target += up * dy)");
        nearly(radiusError(c), 0.0f, "pan: the eye follows a vertical pan too");
    }
    {
        // Panning from an arbitrary orientation. The world axes are useless
        // here; only the camera's own basis gives the right answer.
        //
        // WHAT IS NOT ASSERTED, AND WHY. The obvious property -- that the
        // target slides across the view and never along it -- is true of the
        // horizontal axis and NOT of the vertical one, because `up` is the
        // world up (0, 1, 0), not the camera's true up. Once the camera is
        // pitched, world up is no longer perpendicular to the view direction,
        // so a vertical pan carries a small component along it.
        //
        // That is a convention, not a bug: panning along world up is what keeps
        // the horizon level and stops a pitched camera from rolling as you drag
        // it. Building a true up with cross(right, forward) instead is an
        // equally defensible reading of "pan across the screen", and a student
        // who does that is not wrong -- so nothing here separates the two. See
        // ASSIGNMENTS.md, A3, open questions.
        //
        // What both conventions must agree on is checked below.
        Camera c;
        c.orbit(0.9f, 0.35f);

        const glm::vec3 forward = glm::normalize(c.getTarget() - c.getPosition());

        // Horizontal only. `right` is perpendicular to the view direction in
        // either convention, because both build it from cross(forward, up).
        {
            const glm::vec3 t0 = c.getTarget();
            c.pan(0.4f, 0.0f);
            const glm::vec3 moved = c.getTarget() - t0;

            ok(glm::length(moved) > 1e-3f, "pan: an oblique drag moves the target");
            nearly(glm::dot(moved, forward), 0.0f,
                   "pan: a horizontal drag slides the target ACROSS the view, "
                   "never along it -- screen x has to be rotated through the "
                   "camera's basis whatever its orientation", 1e-3f);
            nearly(glm::length(moved), 0.4f,
                   "pan: the distance moved is the size of the drag, whatever "
                   "the orientation -- the right axis must be NORMALIZED", 1e-3f);
        }

        // Both axes at once. The two are perpendicular to each other in either
        // convention (right is built as a cross product with the up axis), so
        // the total displacement is the length of the drag either way.
        {
            const glm::vec3 t0 = c.getTarget();
            c.pan(0.4f, -0.25f);
            const glm::vec3 moved = c.getTarget() - t0;

            nearly(glm::length(moved), glm::length(glm::vec2(0.4f, 0.25f)),
                   "pan: the right and up axes are perpendicular and both unit "
                   "length, so an oblique drag moves the target by exactly the "
                   "length of the drag", 1e-3f);
            ok(glm::dot(moved, glm::vec3(0.0f, 1.0f, 0.0f)) < 0.0f,
               "pan: a negative vertical delta lowers the target from an "
               "oblique view too -- the up axis is not being confused with the "
               "view direction");
        }

        nearly(radiusError(c), 0.0f, "pan: the eye follows an oblique pan");
        ok(finiteView(c), "pan: the view matrix stays finite");
    }

    // --- the three together --------------------------------------------------
    //
    // Anything can survive one call. This drives all three controls against
    // each other for a while and then checks that the camera is still a camera.
    {
        Camera c;
        for (int i = 0; i < 500; ++i) {
            c.orbit(0.31f * (i % 7 - 3), 0.23f * (i % 5 - 2));
            c.pan(0.05f * (i % 3 - 1), 0.05f * (i % 4 - 2));
            c.zoom(0.4f * (i % 6 - 2));
        }

        ok(finite3(c.getPosition()) && finite3(c.getTarget()),
           "combined: 500 rounds of orbit + pan + zoom leave the eye and target "
           "finite");
        ok(finiteView(c), "combined: the view matrix is still finite");
        ok(std::fabs(c.getPhi()) < PI * 0.5f, "combined: the pitch is still inside the poles");
        ok(c.getRadius() > 0.0f, "combined: the radius is still positive");
        nearly(radiusError(c), 0.0f,
               "combined: the eye is still exactly `radius` from the target -- "
               "the invariant holds after every path through the three controls",
               1e-2f);

        // The centre ray and the view matrix are both derived from the same
        // state by given code, so if the controls have left that state coherent
        // these two have to agree. They disagree the moment updatePosition() is
        // skipped somewhere.
        const Ray centre = c.generateRay(64.0f, 64.0f, 128, 128);
        const glm::vec3 toTarget = glm::normalize(c.getTarget() - c.getPosition());
        nearly(glm::dot(centre.direction, toTarget), 1.0f,
               "combined: the ray through the centre pixel points straight at "
               "the target -- the rasterizer and the ray tracer still agree "
               "about where the camera is", 1e-3f);
    }

    std::printf("\n%d checks, %d failures\n", checks, failures);
    if (failures == 0) { std::printf("ALL TESTS PASSED (0 failures)\n"); return 0; }
    std::printf("TESTS FAILED (%d failures)\n", failures);
    return 1;
}
