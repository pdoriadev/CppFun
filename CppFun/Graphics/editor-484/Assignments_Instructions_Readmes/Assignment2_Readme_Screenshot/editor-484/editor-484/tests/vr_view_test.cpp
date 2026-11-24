// Grader for the `vrcamera` topic: vrViewMatrix().
//
// A3 EXTRA CREDIT, both courses.
//
// Headless, GL-free, and -- the point of the whole arrangement -- HEADSET-FREE.
// It links Camera.o, CameraControl.o, HeadPose.o and VRCamera.o and fabricates
// every pose it tests. A student without a Quest can do this work and know it
// is right; a grader without a Quest can mark it. The headset is needed to
// SEE the result, which is what the demo is for, and is a separate thing from
// knowing the matrix is correct.
//
// PROPERTY-BASED, like camera_test. There is one hand-computed matrix in here
// and it is deliberately the simplest possible case; everything else is stated
// as an invariant, because there are several defensible conventions for where
// play space sits and this suite should not pick a fight with any of them.
//
// -----------------------------------------------------------------------------
// WHAT IT IS ACTUALLY HUNTING
// -----------------------------------------------------------------------------
// Four bugs, and they are the four that VRCamera.cpp's stub comment names,
// because they are the four that a person wearing the headset cannot easily
// tell apart -- all four produce "it looks wrong when I move my head":
//
//   1. the stub, or an unconditional `return orbit.getViewMatrix()`
//   2. eye-to-world returned instead of world-to-camera (missing inverse)
//   3. the eye offset applied on the wrong side, or dropped
//   4. the two eyes swapped
//
// (3) and (4) are the ones worth being careful about. A swapped-eye stereo
// image looks *sharp*. It reads as correct for about thirty seconds and then
// gives the wearer a headache, and nobody debugs a headache. It is caught here
// in a microsecond instead.
//
// -----------------------------------------------------------------------------
// THE ANTI-VACUOUS CHECK
// -----------------------------------------------------------------------------
// The shipped stub returns orbit.getViewMatrix(), which is a legitimate return
// value in one case (head.valid == false). So a check that only ever fed this
// function an invalid pose would pass against a function that does nothing.
// The eye-separation checks below are what make that impossible: the orbit
// camera has one eye, so any implementation that ignores eyeOffsetLocal puts
// both eyes in the same place and fails. Do not remove them.

#include <cstdio>
#include <cmath>
#include <string>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "HeadPose.h"

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

static bool finiteMat(const glm::mat4& m) {
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
            if (!(m[c][r] == m[c][r])) return false;
    return true;
}

// Where the eye ended up, recovered from a world-to-camera matrix the same way
// Camera::setViewOverride() recovers it: invert, and read the translation.
static glm::vec3 eyeOf(const glm::mat4& view) {
    return glm::vec3(glm::inverse(view)[3]);
}

// Which way the eye is looking. OpenGL convention: down its own -Z.
static glm::vec3 forwardOf(const glm::mat4& view) {
    return -glm::normalize(glm::vec3(glm::inverse(view)[2]));
}

static glm::vec3 rightOf(const glm::mat4& view) {
    return glm::normalize(glm::vec3(glm::inverse(view)[0]));
}

// A pose sitting at `p` metres in play space, yawed `yaw` radians about play +Y.
static HeadPose poseAt(const glm::vec3& p, float yaw = 0.0f) {
    HeadPose h = noHeadPose();
    h.valid = true;
    h.headToPlaySpace = glm::translate(glm::mat4(1.0f), p) *
                        glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    return h;
}

int main() {
    std::printf("VR view matrix (A3 extra credit)\n");

    const float PI = 3.14159265358979f;

    // --- no tracking ---------------------------------------------------------
    //
    // First, because it is the one case where "return the orbit matrix" is the
    // right answer, and because a student who has not started should see this
    // one pass. It is not evidence of anything else.
    {
        Camera c;
        HeadPose dead = noHeadPose();          // valid == false
        glm::mat4 v = vrViewMatrix(dead, VR_EYE_LEFT, c, 1.0f);

        ok(finiteMat(v), "no tracking: the returned matrix is finite");
        nearly(glm::length(eyeOf(v) - c.getPosition()), 0.0f,
               "no tracking: falls back to the orbit camera, so the viewport "
               "keeps working when the headset is set down");
    }

    // --- the one hand-computed case -----------------------------------------
    //
    // Head at the play origin, facing play -Z, eyes symmetric, one unit per
    // metre, orbit target at the world origin. Whatever convention is used for
    // placing play space, this case pins the eye to a point that can be worked
    // out on paper.
    {
        Camera c;
        c.setTarget(glm::vec3(0.0f));
        HeadPose h = poseAt(glm::vec3(0.0f));

        glm::mat4 vl = vrViewMatrix(h, VR_EYE_LEFT,  c, 1.0f);
        glm::mat4 vr = vrViewMatrix(h, VR_EYE_RIGHT, c, 1.0f);

        ok(finiteMat(vl) && finiteMat(vr),
           "origin pose: both eye matrices are finite");

        nearly(eyeOf(vl).x, h.eyeOffsetLocal[VR_EYE_LEFT].x,
               "origin pose: the left eye lands at the left eye offset");
        nearly(eyeOf(vr).x, h.eyeOffsetLocal[VR_EYE_RIGHT].x,
               "origin pose: the right eye lands at the right eye offset");
        nearly(eyeOf(vl).y, 0.0f, "origin pose: the left eye is at play height");
        nearly(eyeOf(vl).z, 0.0f, "origin pose: the left eye is at play depth");

        // An untransformed head faces -Z. If this fails the head transform is
        // being inverted somewhere it should not be.
        nearly(glm::dot(forwardOf(vl), glm::vec3(0.0f, 0.0f, -1.0f)), 1.0f,
               "origin pose: an unrotated head looks down world -Z");
    }

    // --- the two eyes are two eyes ------------------------------------------
    //
    // THE anti-vacuous section. An implementation that ignores the eye offset
    // -- including the shipped stub -- puts both eyes in one place and cannot
    // get past this.
    {
        Camera c;
        c.setTarget(glm::vec3(0.0f));
        HeadPose h = poseAt(glm::vec3(0.0f));

        glm::vec3 l = eyeOf(vrViewMatrix(h, VR_EYE_LEFT,  c, 1.0f));
        glm::vec3 r = eyeOf(vrViewMatrix(h, VR_EYE_RIGHT, c, 1.0f));

        ok(glm::length(r - l) > 1e-4f,
           "stereo: the two eyes are in DIFFERENT places -- an implementation "
           "that ignores eyeOffsetLocal fails here and nowhere else");

        nearly(glm::length(r - l), h.ipdMetres,
               "stereo: the eyes are exactly one IPD apart at one unit per metre");

        // Direction, not just separation. This is the swapped-eye check.
        glm::mat4 vl = vrViewMatrix(h, VR_EYE_LEFT, c, 1.0f);
        ok(glm::dot(r - l, rightOf(vl)) > 0.0f,
           "stereo: the RIGHT eye is to the right of the left one -- swap these "
           "and the image still looks sharp, and still makes the wearer ill");
    }

    // --- the head actually moves the eye ------------------------------------
    {
        Camera c;
        c.setTarget(glm::vec3(0.0f));

        glm::vec3 e0 = eyeOf(vrViewMatrix(poseAt(glm::vec3(0.0f)),
                                          VR_EYE_LEFT, c, 1.0f));
        glm::vec3 e1 = eyeOf(vrViewMatrix(poseAt(glm::vec3(0.0f, 0.0f, -2.0f)),
                                          VR_EYE_LEFT, c, 1.0f));

        nearly(glm::length(e1 - e0), 2.0f,
               "walking: stepping two metres moves the eye two world units at "
               "one unit per metre");
        ok(e1.z < e0.z - 1.0f,
           "walking: stepping towards play -Z moves the eye towards world -Z");
    }

    // --- the head actually turns the view -----------------------------------
    {
        Camera c;
        c.setTarget(glm::vec3(0.0f));

        glm::mat4 v0 = vrViewMatrix(poseAt(glm::vec3(0.0f), 0.0f),
                                    VR_EYE_LEFT, c, 1.0f);
        glm::mat4 v1 = vrViewMatrix(poseAt(glm::vec3(0.0f), PI * 0.5f),
                                    VR_EYE_LEFT, c, 1.0f);

        const float cosAngle = glm::dot(forwardOf(v0), forwardOf(v1));
        nearly(cosAngle, 0.0f,
               "turning: a 90-degree yaw of the head turns the view by 90 degrees",
               2e-2f);

        // The eye offset has to ride along with the rotation. If the offset was
        // applied on the wrong side of the head transform, the eyes stay pinned
        // to the room's X axis and this is the check that says so.
        glm::vec3 l = eyeOf(vrViewMatrix(poseAt(glm::vec3(0.0f), PI * 0.5f),
                                         VR_EYE_LEFT, c, 1.0f));
        glm::vec3 r = eyeOf(vrViewMatrix(poseAt(glm::vec3(0.0f), PI * 0.5f),
                                         VR_EYE_RIGHT, c, 1.0f));
        ok(std::fabs((r - l).z) > std::fabs((r - l).x),
           "turning: with the head yawed 90 degrees the eyes separate along Z, "
           "not X -- the eye offset is in HEAD space and must be applied on the "
           "right of the head transform");
    }

    // --- the mouse still has a say ------------------------------------------
    //
    // The whole design is that the headset and the mouse are SEPARATE inputs to
    // the same matrix. Panning has to move the room.
    {
        Camera c;
        c.setTarget(glm::vec3(0.0f));
        glm::vec3 e0 = eyeOf(vrViewMatrix(poseAt(glm::vec3(0.0f)),
                                          VR_EYE_LEFT, c, 1.0f));

        c.setTarget(glm::vec3(3.0f, 1.0f, -2.0f));
        glm::vec3 e1 = eyeOf(vrViewMatrix(poseAt(glm::vec3(0.0f)),
                                          VR_EYE_LEFT, c, 1.0f));

        nearly(glm::length(e1 - e0), glm::length(glm::vec3(3.0f, 1.0f, -2.0f)),
               "placement: moving the orbit target carries play space with it -- "
               "the mouse still decides where the room sits in the scene");
    }

    // --- units --------------------------------------------------------------
    {
        Camera c;
        c.setTarget(glm::vec3(0.0f));
        HeadPose h = poseAt(glm::vec3(0.0f, 0.0f, -1.0f));

        glm::vec3 a = eyeOf(vrViewMatrix(h, VR_EYE_LEFT, c, 1.0f));
        glm::vec3 b = eyeOf(vrViewMatrix(h, VR_EYE_LEFT, c, 4.0f));

        nearly(glm::length(b), 4.0f * glm::length(a),
               "units: four units per metre puts a one-metre step four units away");

        glm::vec3 l = eyeOf(vrViewMatrix(h, VR_EYE_LEFT,  c, 4.0f));
        glm::vec3 r = eyeOf(vrViewMatrix(h, VR_EYE_RIGHT, c, 4.0f));
        nearly(glm::length(r - l), 4.0f * h.ipdMetres,
               "units: the eye separation scales with the units, so the scene "
               "reads as bigger rather than just further away");
    }

    // --- degenerate input ---------------------------------------------------
    //
    // The panel clamps its slider. This function is still called directly, and
    // a singular matrix inverted is a viewport full of NaN that survives every
    // subsequent frame.
    {
        Camera c;
        HeadPose h = poseAt(glm::vec3(0.0f));

        ok(finiteMat(vrViewMatrix(h, VR_EYE_LEFT, c,  0.0f)),
           "degenerate: zero units per metre does not produce NaN");
        ok(finiteMat(vrViewMatrix(h, VR_EYE_LEFT, c, -1.0f)),
           "degenerate: a negative scale does not produce NaN");
    }

    // --- it is a view matrix, not something that looks like one -------------
    //
    // At one unit per metre the result must be a rigid transform: no scale, no
    // shear, no reflection. det == -1 rather than +1 means a handedness flip,
    // which renders a mirror image that is very hard to notice and impossible
    // to explain.
    {
        Camera c;
        c.setTarget(glm::vec3(0.4f, -1.0f, 2.0f));
        glm::mat4 v = vrViewMatrix(poseAt(glm::vec3(0.3f, 1.6f, -0.8f), 0.7f),
                                   VR_EYE_RIGHT, c, 1.0f);
        glm::mat3 R(v);

        nearly(glm::length(R[0]), 1.0f, "rigid: the view basis is unit length (X)");
        nearly(glm::length(R[1]), 1.0f, "rigid: the view basis is unit length (Y)");
        nearly(glm::length(R[2]), 1.0f, "rigid: the view basis is unit length (Z)");
        nearly(glm::dot(R[0], R[1]), 0.0f, "rigid: the view basis is orthogonal");
        nearly(glm::determinant(R), 1.0f,
               "rigid: the view matrix does not flip handedness -- a determinant "
               "of -1 renders a mirror image nobody spots until they read text");
    }

    // --- 500 poses ----------------------------------------------------------
    {
        Camera c;
        std::srand(20260907u);
        bool allFinite = true, allSeparated = true;
        auto rnd = [](float lo, float hi) {
            return lo + (hi - lo) * (float(std::rand()) / float(RAND_MAX));
        };

        for (int i = 0; i < 500; ++i) {
            c.setTarget(glm::vec3(rnd(-20, 20), rnd(-20, 20), rnd(-20, 20)));
            c.orbit(rnd(-3, 3), rnd(-3, 3));
            c.zoom(rnd(-5, 5));

            HeadPose h = poseAt(glm::vec3(rnd(-3, 3), rnd(0, 2), rnd(-3, 3)),
                                rnd(-7, 7));
            const float s = rnd(0.05f, 50.0f);

            glm::mat4 vl = vrViewMatrix(h, VR_EYE_LEFT,  c, s);
            glm::mat4 vr = vrViewMatrix(h, VR_EYE_RIGHT, c, s);

            if (!finiteMat(vl) || !finiteMat(vr)) allFinite = false;
            if (glm::length(eyeOf(vr) - eyeOf(vl)) < 1e-5f) allSeparated = false;
        }

        ok(allFinite,  "500 random poses: no NaN anywhere");
        ok(allSeparated, "500 random poses: the eyes never collapse together");
    }

    std::printf("\n%d checks, %d failures\n", checks, failures);
    if (failures == 0) {
        std::printf("ALL TESTS PASSED (0 failures)\n");
    } else {
        std::printf("\nThis is EXTRA CREDIT. A failure here costs nothing you\n"
                    "were going to have anyway -- the rest of A3 is graded by\n"
                    "./obj_parser_test and ./camera_test and neither of them\n"
                    "links this file.\n");
    }
    return failures == 0 ? 0 : 1;
}
