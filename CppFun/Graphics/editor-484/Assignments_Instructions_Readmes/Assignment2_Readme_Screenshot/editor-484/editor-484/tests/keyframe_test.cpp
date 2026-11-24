// Keyframe evaluation -- assertions for CPSC 484 Assignment 4.
//
// Headless: no window, no GL context. AnimationTrack::evaluate() is a pure
// function of the keys and t, which is the whole reason keyframe animation is
// testable in a way the particle systems are not.
//
// This suite is the objective part of the assignment's grade. Every case here
// corresponds to a line of the contract in include/Keyframe.h.

#include <cstdio>
#include <cmath>
#include <string>

#include <glm/glm.hpp>
#include "Keyframe.h"
#include "ShapeManager.h"
#include "Camera.h"
#include "SceneIO.h"
#include "Cube.h"
#include "gl_context.h"

static int failures = 0;
static int checks   = 0;

static void ok(bool cond, const std::string& what) {
    ++checks;
    if (!cond) {
        ++failures;
        std::printf("  FAIL  %s\n", what.c_str());
    }
}

static void nearly(float a, float b, const std::string& what, float eps = 1e-4f) {
    ++checks;
    if (std::fabs(a - b) > eps) {
        ++failures;
        std::printf("  FAIL  %s   (got %.6f, expected %.6f)\n",
                    what.c_str(), a, b);
    }
}

static void nearlyVec(const glm::vec3& a, const glm::vec3& b,
                      const std::string& what, float eps = 1e-4f) {
    ++checks;
    if (std::fabs(a.x - b.x) > eps ||
        std::fabs(a.y - b.y) > eps ||
        std::fabs(a.z - b.z) > eps) {
        ++failures;
        std::printf("  FAIL  %s\n"
                    "         got      (%.6f, %.6f, %.6f)\n"
                    "         expected (%.6f, %.6f, %.6f)\n",
                    what.c_str(), a.x, a.y, a.z, b.x, b.y, b.z);
    }
}

// A two-key track: the unit almost every case below is built from.
static AnimationTrack twoKeyTrack() {
    AnimationTrack tr(1);
    tr.addKey(Keyframe(0.0f,
                       glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec3(1.0f, 1.0f, 1.0f)));
    tr.addKey(Keyframe(2.0f,
                       glm::vec3(10.0f, -4.0f, 6.0f),
                       glm::vec3(90.0f, 0.0f, 0.0f),
                       glm::vec3(3.0f, 3.0f, 3.0f)));
    return tr;
}

int main() {
    std::printf("Keyframe evaluation\n");
    Keyframe out;

    // =======================================================================
    // STORAGE FIRST -- and deliberately first.
    // =======================================================================
    // addKey, removeKeyNear and moveKey maintain one invariant: the keys are
    // sorted by time and no two share an instant. evaluate() depends on it.
    //
    // If these fail, EXPECT THE INTERPOLATION CHECKS BELOW TO FAIL TOO, and fix
    // these first -- an unsorted vector breaks the bracketing search, and two
    // keys at one instant divide by zero. The cascade runs one way only, which
    // is why this block runs before anything else: the first failures you see
    // are the cause, not the symptom.
    const int failuresBeforeStorage = failures;
    {
        AnimationTrack tr(1);
        tr.addKey(Keyframe(2.0f, glm::vec3(2.0f), glm::vec3(0.0f), glm::vec3(1.0f)));
        tr.addKey(Keyframe(0.0f, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f)));
        tr.addKey(Keyframe(1.0f, glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(1.0f)));

        ok(tr.keys.size() == 3, "storage: three keys inserted, three kept");
        bool sorted = true;
        for (size_t i = 1; i < tr.keys.size(); ++i) {
            if (tr.keys[i].time < tr.keys[i - 1].time) sorted = false;
        }
        ok(sorted, "storage: addKey keeps the keys sorted whatever order they arrive in");

        // Same instant: replace, never append. Two keys at one time make
        // evaluate() order-dependent and divide by zero between them.
        tr.addKey(Keyframe(1.0f, glm::vec3(9.0f), glm::vec3(0.0f), glm::vec3(1.0f)));
        ok(tr.keys.size() == 3,
           "storage: a key at an existing time REPLACES it -- two keys at one "
           "instant divide by zero inside evaluate()");
        if (tr.keys.size() == 3) {
            nearly(tr.keys[1].position.x, 9.0f,
                   "storage: and the replacement is the key that survives");
        } else { ++checks; ++failures; }
    }
    {
        // moveKey's contract: retime, keep sorted, and RETURN WHERE IT ENDED UP.
        AnimationTrack tr(1);
        for (int i = 0; i < 4; ++i) {
            tr.addKey(Keyframe(static_cast<float>(i),
                               glm::vec3(static_cast<float>(i)),
                               glm::vec3(0.0f), glm::vec3(1.0f)));
        }

        // Drag key 0 (position.x == 0) past two neighbours, to t = 2.5.
        const int landed = tr.moveKey(0, 2.5f);
        ok(landed == 2,
           "storage: moveKey returns the index the key ENDED UP at -- a drag "
           "past a neighbour re-sorts the vector underneath the caller");
        ok(tr.keys.size() == 4, "storage: moving a key does not lose one");
        if (landed >= 0 && static_cast<size_t>(landed) < tr.keys.size()) {
            nearly(tr.keys[landed].position.x, 0.0f,
                   "storage: the returned index still points at the SAME "
                   "keyframe, not whatever slid into its old slot");
            nearly(tr.keys[landed].time, 2.5f, "storage: and it is at its new time");
        } else { checks += 2; failures += 2; }

        bool sorted = true;
        for (size_t i = 1; i < tr.keys.size(); ++i) {
            if (tr.keys[i].time < tr.keys[i - 1].time) sorted = false;
        }
        ok(sorted, "storage: the list is still sorted after a move");
    }
    {
        AnimationTrack tr(1);
        for (int i = 0; i < 3; ++i) {
            tr.addKey(Keyframe(static_cast<float>(i), glm::vec3(static_cast<float>(i)),
                               glm::vec3(0.0f), glm::vec3(1.0f)));
        }
        // Drop key 0 exactly onto key 2. The moved key wins; the list shrinks.
        const int landed = tr.moveKey(0, 2.0f);
        ok(tr.keys.size() == 2,
           "storage: dropping a key onto another REPLACES it -- same rule as "
           "addKey, same reason");
        if (landed >= 0 && static_cast<size_t>(landed) < tr.keys.size()) {
            nearly(tr.keys[landed].position.x, 0.0f,
                   "storage: the key that was MOVED is the one that survives");
        } else { ++checks; ++failures; }

        ok(tr.moveKey(99, 1.0f) == -1,
           "storage: moveKey on an out-of-range index returns -1");

        AnimationTrack neg(1);
        neg.addKey(Keyframe(1.0f, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f)));
        neg.moveKey(0, -5.0f);
        ok(!neg.keys.empty() && neg.keys[0].time >= 0.0f,
           "storage: moveKey clamps at zero -- there is no negative time");
    }
    {
        // removeKeyNear takes the NEAREST key within eps, not the first one it
        // walks past. With a generous eps the two differ, and Delete key passes
        // eps = 0.05 precisely so it can find the key you meant.
        AnimationTrack tr(1);
        tr.addKey(Keyframe(1.00f, glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(1.0f)));
        tr.addKey(Keyframe(1.04f, glm::vec3(2.0f), glm::vec3(0.0f), glm::vec3(1.0f)));

        ok(tr.removeKeyNear(1.045f, 0.05f), "storage: removeKeyNear finds a key in range");
        ok(tr.keys.size() == 1, "storage: and removes exactly one");
        if (tr.keys.size() == 1) {
            nearly(tr.keys[0].time, 1.00f,
                   "storage: it removed the NEAREST key (1.04), not the first "
                   "one within eps (1.00)");
        } else { ++checks; ++failures; }

        ok(!tr.removeKeyNear(50.0f, 0.05f),
           "storage: nothing in range removes nothing and says so");
    }
    if (failures > failuresBeforeStorage) {
        std::printf("\n  >>> STORAGE IS BROKEN. Fix addKey / moveKey / removeKeyNear\n"
                    "  >>> before reading the interpolation failures below -- most\n"
                    "  >>> of them are downstream of these.\n\n");
    }

    // --- empty and single-key tracks ---------------------------------------
    {
        AnimationTrack empty(1);
        ok(!empty.evaluate(0.0f, out), "an empty track evaluates to false");
        nearly(empty.duration(), 0.0f, "an empty track has zero duration");
    }
    {
        AnimationTrack one(1);
        one.addKey(Keyframe(5.0f, glm::vec3(1, 2, 3), glm::vec3(10, 20, 30),
                            glm::vec3(2, 2, 2)));
        ok(one.evaluate(0.0f, out), "a single-key track evaluates to true");
        nearlyVec(out.position, glm::vec3(1, 2, 3),
                  "a single key holds its pose before its own time");
        ok(one.evaluate(99.0f, out), "single key, t far past");
        nearlyVec(out.position, glm::vec3(1, 2, 3),
                  "a single key holds its pose after its own time");
        nearly(one.duration(), 0.0f, "one key is zero duration");
    }

    // --- clamping at the ends ----------------------------------------------
    {
        AnimationTrack tr = twoKeyTrack();
        ok(tr.evaluate(-5.0f, out), "t before the first key evaluates");
        nearlyVec(out.position, glm::vec3(0, 0, 0),
                  "t before the start clamps to the first key (no extrapolation)");
        nearlyVec(out.scale, glm::vec3(1, 1, 1), "scale clamps at the start too");

        ok(tr.evaluate(50.0f, out), "t after the last key evaluates");
        nearlyVec(out.position, glm::vec3(10, -4, 6),
                  "t after the end clamps to the last key (no extrapolation)");
        nearlyVec(out.scale, glm::vec3(3, 3, 3), "scale clamps at the end too");
    }

    // --- interpolation ------------------------------------------------------
    {
        AnimationTrack tr = twoKeyTrack();
        ok(tr.evaluate(1.0f, out), "midpoint evaluates");
        nearlyVec(out.position, glm::vec3(5.0f, -2.0f, 3.0f),
                  "position at the midpoint is the average of the two keys");
        nearlyVec(out.scale, glm::vec3(2.0f, 2.0f, 2.0f),
                  "scale at the midpoint");
        nearlyVec(out.rotation, glm::vec3(45.0f, 0.0f, 0.0f),
                  "rotation at the midpoint");
        nearly(out.time, 1.0f, "out.time is the evaluated time");

        tr.evaluate(0.5f, out);
        nearlyVec(out.position, glm::vec3(2.5f, -1.0f, 1.5f),
                  "quarter point interpolates linearly");

        tr.evaluate(0.0f, out);
        nearlyVec(out.position, glm::vec3(0, 0, 0), "exactly on the first key");
        tr.evaluate(2.0f, out);
        nearlyVec(out.position, glm::vec3(10, -4, 6), "exactly on the last key");
    }

    // --- three keys: the bracketing search has to pick the right pair -------
    {
        AnimationTrack tr(1);
        tr.addKey(Keyframe(0.0f, glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(1)));
        tr.addKey(Keyframe(1.0f, glm::vec3(10, 0, 0), glm::vec3(0), glm::vec3(1)));
        tr.addKey(Keyframe(3.0f, glm::vec3(10, 20, 0), glm::vec3(0), glm::vec3(1)));

        tr.evaluate(0.5f, out);
        nearlyVec(out.position, glm::vec3(5, 0, 0),
                  "t in the first interval uses keys 0 and 1");
        tr.evaluate(2.0f, out);
        nearlyVec(out.position, glm::vec3(10, 10, 0),
                  "t in the second interval uses keys 1 and 2");
        tr.evaluate(1.0f, out);
        nearlyVec(out.position, glm::vec3(10, 0, 0),
                  "t exactly on the middle key gives that key");
        nearly(tr.duration(), 3.0f, "duration spans first to last key");
    }

    // --- keys inserted out of order must still be sorted --------------------
    {
        AnimationTrack tr(1);
        tr.addKey(Keyframe(2.0f, glm::vec3(20, 0, 0), glm::vec3(0), glm::vec3(1)));
        tr.addKey(Keyframe(0.0f, glm::vec3(0, 0, 0),  glm::vec3(0), glm::vec3(1)));
        tr.addKey(Keyframe(1.0f, glm::vec3(10, 0, 0), glm::vec3(0), glm::vec3(1)));

        ok(tr.keys.size() == 3, "three distinct times give three keys");
        ok(tr.keys[0].time < tr.keys[1].time && tr.keys[1].time < tr.keys[2].time,
           "addKey keeps the keys sorted by time");
        tr.evaluate(1.5f, out);
        nearlyVec(out.position, glm::vec3(15, 0, 0),
                  "out-of-order insertion still interpolates correctly");
    }

    // --- a key at an existing time replaces rather than duplicates ----------
    {
        AnimationTrack tr = twoKeyTrack();
        tr.addKey(Keyframe(2.0f, glm::vec3(99, 99, 99), glm::vec3(0), glm::vec3(1)));
        ok(tr.keys.size() == 2, "re-keying an existing time replaces it");
        tr.evaluate(2.0f, out);
        nearlyVec(out.position, glm::vec3(99, 99, 99),
                  "the replacement key is the one that takes effect");
    }

    // --- Euler interpolation goes the short way round -----------------------
    //
    // The case students most often get wrong. Naive lerp from 350 to 10 sweeps
    // 340 degrees backwards; the correct answer crosses the seam.
    {
        AnimationTrack tr(1);
        tr.addKey(Keyframe(0.0f, glm::vec3(0), glm::vec3(350.0f, 0, 0), glm::vec3(1)));
        tr.addKey(Keyframe(1.0f, glm::vec3(0), glm::vec3(10.0f, 0, 0),  glm::vec3(1)));

        tr.evaluate(0.5f, out);
        // 350 + 0.5 * (+20) = 360, not 350 + 0.5 * (-340) = 180.
        nearly(out.rotation.x, 360.0f,
               "rotation crosses the 0/360 seam the short way, not backwards");

        tr.evaluate(0.25f, out);
        nearly(out.rotation.x, 355.0f, "quarter of the short arc");
    }
    {
        // And the other direction.
        AnimationTrack tr(1);
        tr.addKey(Keyframe(0.0f, glm::vec3(0), glm::vec3(0, 10.0f, 0), glm::vec3(1)));
        tr.addKey(Keyframe(1.0f, glm::vec3(0), glm::vec3(0, 350.0f, 0), glm::vec3(1)));
        tr.evaluate(0.5f, out);
        nearly(out.rotation.y, 0.0f,
               "10 -> 350 interpolates backwards through 0, not forwards through 180");
    }
    {
        // Exactly 180 apart is the ambiguous case; it must not blow up, and it
        // must land on one of the two half-way answers.
        AnimationTrack tr(1);
        tr.addKey(Keyframe(0.0f, glm::vec3(0), glm::vec3(0, 0, 0),     glm::vec3(1)));
        tr.addKey(Keyframe(1.0f, glm::vec3(0), glm::vec3(0, 0, 180.0f), glm::vec3(1)));
        tr.evaluate(0.5f, out);
        ok(std::fabs(std::fabs(out.rotation.z) - 90.0f) < 1e-3f,
           "a 180-degree turn interpolates to +/-90 at the midpoint");
    }
    {
        // Multi-turn values must not need many wrap iterations or lose accuracy.
        AnimationTrack tr(1);
        tr.addKey(Keyframe(0.0f, glm::vec3(0), glm::vec3(0, 0, 0),      glm::vec3(1)));
        tr.addKey(Keyframe(1.0f, glm::vec3(0), glm::vec3(0, 0, 1080.0f), glm::vec3(1)));
        tr.evaluate(0.5f, out);
        nearly(out.rotation.z, 0.0f,
               "1080 degrees is three full turns, so the short arc is zero");
    }

    // --- looping ------------------------------------------------------------
    {
        AnimationTrack tr = twoKeyTrack();     // spans t in [0, 2]
        tr.loop = true;

        tr.evaluate(3.0f, out);
        nearlyVec(out.position, glm::vec3(5.0f, -2.0f, 3.0f),
                  "t = 3 on a looping [0,2] track wraps to t = 1");

        tr.evaluate(4.0f, out);
        nearlyVec(out.position, glm::vec3(0, 0, 0),
                  "t = 4 wraps to exactly t = 0");

        tr.evaluate(-1.0f, out);
        nearlyVec(out.position, glm::vec3(5.0f, -2.0f, 3.0f),
                  "negative t wraps forwards, not to the first key");

        tr.loop = false;
        tr.evaluate(3.0f, out);
        nearlyVec(out.position, glm::vec3(10, -4, 6),
                  "with loop off the same t clamps to the last key");
    }

    // --- a looping track whose keys do not start at zero --------------------
    {
        AnimationTrack tr(1);
        tr.addKey(Keyframe(10.0f, glm::vec3(0, 0, 0),  glm::vec3(0), glm::vec3(1)));
        tr.addKey(Keyframe(12.0f, glm::vec3(20, 0, 0), glm::vec3(0), glm::vec3(1)));
        tr.loop = true;
        tr.evaluate(13.0f, out);
        nearlyVec(out.position, glm::vec3(10, 0, 0),
                  "wrapping is relative to the first key, not to zero");
    }

    // --- removal ------------------------------------------------------------
    {
        AnimationTrack tr = twoKeyTrack();
        ok(!tr.removeKeyNear(1.0f), "removeKeyNear misses when nothing is close");
        ok(tr.removeKeyNear(2.0f), "removeKeyNear finds an exact time");
        ok(tr.keys.size() == 1, "the key is gone");
        tr.evaluate(1.0f, out);
        nearlyVec(out.position, glm::vec3(0, 0, 0),
                  "what is left behaves as a single-key track");
    }

    // --- lookup by id, not by position --------------------------------------
    //
    // Regression. ShapeManager::getShapeById used to be `return shapes[id];` --
    // a positional lookup with an id-shaped name. Both of its callers passed a
    // loop counter, so it went unnoticed until AnimationTimeline passed a real
    // id and indexed past the end of the vector. Ids start at 1 and keep
    // climbing as shapes are deleted; positions do not.
    if (makeHeadlessContext()) {
        ShapeManager sm;
        Cube* a = new Cube(1.0f, 0, 0, 1.0f, 2, 10);
        Cube* b = new Cube(2.0f, 0, 0, 1.0f, 3, 20);
        sm.addShape(a);
        sm.addShape(b);

        // Two shapes, ids 10 and 20 -- both far past the end of a size-2 vector.
        ok(sm.getShapeById(10) == a, "id 10 finds the first shape, not shapes[10]");
        ok(sm.getShapeById(20) == b, "id 20 finds the second shape");
        ok(sm.getShapeById(0)  == 0, "an id that is not present returns null");
        ok(sm.getShapeById(1)  == 0, "and is not silently read as a position");

        ok(sm.getShapeAt(0) == a, "getShapeAt is the positional accessor");
        ok(sm.getShapeAt(1) == b, "getShapeAt, second");
        ok(sm.getShapeAt(2) == 0, "getShapeAt returns null past the end rather than reading off it");

        // And the timeline drives a shape through it, which is how the crash
        // was actually reached: apply() runs every frame, for every track.
        AnimationTimeline tl;
        tl.trackFor(20).addKey(Keyframe(0.0f, glm::vec3(5, 6, 7),
                                        glm::vec3(0), glm::vec3(1)));
        tl.trackFor(20).addKey(Keyframe(1.0f, glm::vec3(5, 6, 7),
                                        glm::vec3(0), glm::vec3(1)));
        tl.currentTime = 0.5f;
        tl.apply(sm);
        nearlyVec(glm::vec3(b->getX(), b->getY(), b->getZ()), glm::vec3(5, 6, 7),
                  "a track keyed on id 20 moves the shape with id 20");
        nearlyVec(glm::vec3(a->getX(), a->getY(), a->getZ()), glm::vec3(1, 0, 0),
                  "and leaves the untracked shape alone");

        // A track referencing a deleted shape must be skipped, not crash.
        tl.trackFor(999).addKey(Keyframe(0.0f, glm::vec3(0), glm::vec3(0),
                                         glm::vec3(1)));
        tl.apply(sm);
        ok(true, "a track whose shape no longer exists is skipped");
    }

    // --- timeline length is independent of the keys -------------------------
    //
    // Regression. The scrub range used to be derived from duration() -- the
    // extent of the keys -- which made it impossible to move the playhead past
    // the last key, so a second key froze the timeline and a third could never
    // be placed. With no keys the range collapsed to a hardcoded one second.
    {
        AnimationTimeline tl;
        ok(tl.length == AnimationTimeline::kDefaultLength,
           "a fresh timeline has a usable length before any keys exist");
        ok(tl.length > 1.0f,
           "and it is not the old one-second fallback");
        nearly(tl.duration(), 0.0f, "with no keys the content extent is zero");

        // Content extent and scrub range are separate values.
        tl.trackFor(1).addKey(Keyframe(0.5f, glm::vec3(0), glm::vec3(0), glm::vec3(1)));
        nearly(tl.duration(), 0.5f, "duration follows the last key");
        ok(tl.length > tl.duration(),
           "length stays wider than the content, so the playhead can move past it");

        // Keying beyond the end widens rather than refusing.
        tl.ensureLength(12.0f);
        nearly(tl.length, 12.0f, "ensureLength grows the timeline");
        tl.ensureLength(3.0f);
        nearly(tl.length, 12.0f, "and never shrinks it");
    }
    {
        // Playback loops over the length, not the keys: a shape whose last key
        // is at t=2 on a 10-second timeline holds that pose for 8 more seconds.
        if (makeHeadlessContext()) {
            ShapeManager sm;
            Cube* c = new Cube(0, 0, 0, 1.0f, 2, 3);
            sm.addShape(c);

            AnimationTimeline tl;
            tl.length = 10.0f;
            tl.trackFor(3).addKey(Keyframe(0.0f, glm::vec3(0, 0, 0),
                                           glm::vec3(0), glm::vec3(1)));
            tl.trackFor(3).addKey(Keyframe(2.0f, glm::vec3(4, 0, 0),
                                           glm::vec3(0), glm::vec3(1)));
            tl.playing = true;
            tl.currentTime = 9.5f;
            tl.update(0.25f, sm);
            nearly(tl.currentTime, 9.75f,
                   "playback runs past the last key, out to the full length");
            ok(tl.playing, "and keeps playing");

            tl.update(0.5f, sm);
            ok(tl.currentTime < 1.0f,
               "crossing the length wraps back to the start, not to the last key");
        }
    }

    // --- scene round trip ---------------------------------------------------
    //
    // A track that does not survive save/load is not usable in an assignment,
    // however correct evaluate() is. Needs a GL context because Cube's
    // constructor creates VAOs.
    if (makeHeadlessContext()) {
        ShapeManager sm;
        Camera cam;
        Cube* c = new Cube(0.0f, 0.0f, 0.0f, 1.0f, 2, 7);
        sm.addShape(c);
        sm.setShapeCounter(8);

        AnimationTimeline tl;
        tl.length = 7.5f;
        AnimationTrack& tr = tl.trackFor(7);
        tr.loop = true;
        // The scene file IS the deliverable for this assignment, so the easing
        // curves have to survive it. An animation that loses them on load is
        // not the animation the student made.
        tr.addKey(Keyframe(0.0f,  glm::vec3(1.5f, 0, 0),  glm::vec3(0, 45.0f, 0),
                           glm::vec3(1, 1, 1)));
        tr.addKey(Keyframe(1.25f, glm::vec3(0, 2.25f, 0), glm::vec3(0, 350.0f, 0),
                           glm::vec3(2, 0.5f, 1)));
        tr.keys[0].easing = EASE_IN_OUT;

        const std::string path = "keyframe_roundtrip.scene";
        ok(SceneIO::save(path, sm, cam, 0, &tl), "scene with a track saves");

        ShapeManager sm2;
        Camera cam2;
        AnimationTimeline tl2;
        SceneIO::Result r = SceneIO::load(path, sm2, cam2, 0, &tl2);
        ok(r.ok, "scene with a track loads");
        ok(tl2.tracks.size() == 1, "one track came back");
        nearly(tl2.length, 7.5f, "the timeline length round-trips");

        if (tl2.tracks.size() == 1) {
            const AnimationTrack& got = tl2.tracks[0];
            ok(got.shapeId == 7, "the track kept its shape id");
            ok(got.loop, "the per-track loop flag round-trips");
            ok(got.keys.size() == 2, "both keys came back");
            if (got.keys.size() == 2) {
                ok(got.keys[0].easing == EASE_IN_OUT,
                   "a key's easing curve round-trips");
                ok(got.keys[1].easing == EASE_LINEAR,
                   "and a key left at the default comes back as the default");
            } else { checks += 2; failures += 2; }

            // Exact equality, not nearly(): floatToString round-trips a float
            // through the shortest representation that reads back identically,
            // so a saved key must reload bit-for-bit. This is the same
            // guarantee dirty_state_test asserts for shapes.
            if (got.keys.size() == 2) {
                ok(got.keys[0].time == 0.0f && got.keys[1].time == 1.25f,
                   "key times survive exactly");
                ok(got.keys[0].position == glm::vec3(1.5f, 0, 0) &&
                   got.keys[1].position == glm::vec3(0, 2.25f, 0),
                   "key positions survive exactly");
                ok(got.keys[1].rotation == glm::vec3(0, 350.0f, 0),
                   "key rotations survive exactly");
                ok(got.keys[1].scale == glm::vec3(2, 0.5f, 1),
                   "key scales survive exactly");
            }

            // And evaluating the reloaded track agrees with the original.
            Keyframe a, b;
            tr.evaluate(0.6f, a);
            got.evaluate(0.6f, b);
            nearlyVec(b.position, a.position, "reloaded track evaluates identically");
            nearlyVec(b.rotation, a.rotation, "reloaded rotation evaluates identically");
        }

        std::remove(path.c_str());
    } else {
        std::printf("  (no GL context -- scene round-trip skipped)\n");
    }

    // --- key navigation (the Key< / Key> transport buttons) -----------------
    //
    // Given code, so this passes in every tree. It is here rather than in the
    // renderer because the buttons cannot be tested through ImGui, and the
    // interesting part is not the drawing -- it is the epsilon and the
    // all-tracks fallback, both of which have a wrong answer that looks like a
    // dead button.
    {
        std::printf("Key navigation\n");

        // A predicate, unlike the file's nearly() above, which is itself an
        // assertion -- these checks need the comparison inside ok().
        struct Close {
            static bool at(float a, float b) { return std::fabs(a - b) < 1e-4f; }
        };

        AnimationTimeline tl;
        AnimationTrack& a = tl.trackFor(1);
        a.addKey(Keyframe(0.0f,  glm::vec3(0), glm::vec3(0), glm::vec3(1)));
        a.addKey(Keyframe(1.0f,  glm::vec3(0), glm::vec3(0), glm::vec3(1)));
        a.addKey(Keyframe(3.0f,  glm::vec3(0), glm::vec3(0), glm::vec3(1)));

        AnimationTrack& b = tl.trackFor(2);
        b.addKey(Keyframe(2.0f,  glm::vec3(0), glm::vec3(0), glm::vec3(1)));

        // trackFor may reallocate the vector, so the references above are only
        // safe until the next call. Re-find them now that both exist.
        const AnimationTrack* trA = tl.findTrack(1);
        const AnimationTrack* trB = tl.findTrack(2);

        float t = -1.0f;

        ok(tl.nearestKey(trA, 0.5f, true, t) && Close::at(t, 1.0f),
           "forward from 0.5 on track A finds 1.0");
        ok(tl.nearestKey(trA, 0.5f, false, t) && Close::at(t, 0.0f),
           "backward from 0.5 finds 0.0");

        // The epsilon. After a jump the playhead sits exactly on a key, and
        // without the tolerance the next press finds that same key again --
        // the button looks broken.
        ok(tl.nearestKey(trA, 1.0f, true, t) && Close::at(t, 3.0f),
           "standing ON a key, forward finds the NEXT one, not itself");
        ok(tl.nearestKey(trA, 1.0f, false, t) && Close::at(t, 0.0f),
           "and backward finds the previous one, not itself");

        ok(!tl.nearestKey(trA, 3.0f, true, t),
           "past the last key there is nothing forward -- the caller clamps to "
           "the timeline length rather than jumping somewhere arbitrary");
        ok(!tl.nearestKey(trA, 0.0f, false, t),
           "and nothing backward from the first");

        // Restricting to one track must actually restrict: track B's key at
        // 2.0 sits between A's 1.0 and 3.0, so a search that leaked across
        // tracks would return it here.
        ok(tl.nearestKey(trA, 1.5f, true, t) && Close::at(t, 3.0f),
           "a per-track search ignores the other track's key in between");
        ok(tl.nearestKey(trB, 0.0f, true, t) && Close::at(t, 2.0f),
           "and finds its own");

        // The fallback: with nothing selected the buttons search everywhere,
        // which is what stops them being dead.
        ok(tl.nearestKey(0, 1.5f, true, t) && Close::at(t, 2.0f),
           "with no track given, the search spans every track");
        ok(tl.nearestKey(0, 2.5f, false, t) && Close::at(t, 2.0f),
           "in both directions");

        AnimationTimeline emptyTl;
        ok(!emptyTl.nearestKey(0, 0.0f, true, t),
           "an empty timeline reports no key rather than reading a track");
    }

    // --- retiming a key (what the timeline drag does) ------------------------
    //
    // Given code, so this runs in every tree. The drag itself cannot be tested
    // through ImGui; this is the part underneath it, and it is where the two
    // things that go wrong actually live: the index moving out from under the
    // caller, and two keys ending up at one instant.
    {
        std::printf("Key retiming\n");

        struct Close {
            static bool at(float a, float b) { return std::fabs(a - b) < 1e-4f; }
        };

        // Reordering: drag the middle key past the last one.
        {
            AnimationTrack tr(1);
            tr.addKey(Keyframe(0.0f, glm::vec3(1), glm::vec3(0), glm::vec3(1)));
            tr.addKey(Keyframe(1.0f, glm::vec3(2), glm::vec3(0), glm::vec3(1)));
            tr.addKey(Keyframe(2.0f, glm::vec3(3), glm::vec3(0), glm::vec3(1)));

            const int now = tr.moveKey(1, 3.0f);
            ok(now == 2,
               "a key dragged past its neighbour reports its NEW index -- a "
               "caller holding the old one would start dragging a different key "
               "halfway through the gesture");
            ok(tr.keys.size() == 3, "and nothing is lost");
            ok(Close::at(tr.keys[2].time, 3.0f), "it landed at the requested time");
            nearlyVec(tr.keys[2].position, glm::vec3(2),
                      "and it is the same keyframe, pose intact");
            ok(tr.keys[0].time < tr.keys[1].time && tr.keys[1].time < tr.keys[2].time,
               "the track is still sorted");
        }

        // Collision: drop one key onto another.
        {
            AnimationTrack tr(1);
            tr.addKey(Keyframe(0.0f, glm::vec3(1), glm::vec3(0), glm::vec3(1)));
            tr.addKey(Keyframe(1.0f, glm::vec3(2), glm::vec3(0), glm::vec3(1)));
            tr.addKey(Keyframe(2.0f, glm::vec3(3), glm::vec3(0), glm::vec3(1)));

            const int now = tr.moveKey(0, 2.0f);
            ok(tr.keys.size() == 2,
               "dropping a key onto another replaces it rather than leaving two "
               "at one instant -- which would divide by zero interpolating "
               "between them");
            ok(now >= 0 && Close::at(tr.keys[now].time, 2.0f), "at the target time");
            nearlyVec(tr.keys[now].position, glm::vec3(1),
                      "and the MOVED key wins, not the one it landed on");
        }

        // A move that changes nothing must not consume the key.
        {
            AnimationTrack tr(1);
            tr.addKey(Keyframe(1.0f, glm::vec3(5), glm::vec3(0), glm::vec3(1)));
            const int now = tr.moveKey(0, 1.0f);
            ok(tr.keys.size() == 1 && now == 0,
               "moving a key onto its own time is a no-op, not a self-collision");
        }

        // Clamping and bounds.
        {
            AnimationTrack tr(1);
            tr.addKey(Keyframe(1.0f, glm::vec3(0), glm::vec3(0), glm::vec3(1)));
            tr.moveKey(0, -5.0f);
            ok(Close::at(tr.keys[0].time, 0.0f), "negative time clamps to 0");
            ok(tr.moveKey(9, 1.0f) == -1, "an out-of-range index reports -1");
            ok(tr.keys.size() == 1, "and changes nothing");
        }
    }

    // --- two bugs found by using the thing ----------------------------------
    if (makeHeadlessContext()) {
        std::printf("Regressions\n");

        // 1. Scrubbing must not untick "Uniform scaling".
        //
        // apply() used to force useUniformScaling(false) on every keyed shape,
        // every frame of a scrub. The flag is user state, not animation state,
        // and a key captured from a uniformly-scaled shape holds three equal
        // numbers anyway -- so the uniform path reproduces it exactly.
        {
            ShapeManager sm;
            Cube* c = new Cube(0.0f, 0.0f, 0.0f, 1.0f, 0, 1);
            c->setScale(2.0f);                       // uniform
            sm.addShape(c);
            sm.setShapeCounter(2);

            AnimationTimeline tl;
            tl.keyShapeAt(sm, 1, 0.0f);
            tl.keyShapeAt(sm, 1, 1.0f);

            ok(c->isUsingUniformScaling(), "a uniformly scaled shape starts uniform");

            tl.currentTime = 0.5f;
            tl.apply(sm);

            ok(c->isUsingUniformScaling(),
               "and is STILL uniform after the playhead moves -- scrubbing must "
               "not untick the checkbox in the properties panel");
            // getScale(), not getNonUniformScale(): the uniform path stores
            // into its own field and leaves scaleX/Y/Z alone, which is exactly
            // why forcing the non-uniform path was a visible change and not a
            // harmless one.
            ok(std::fabs(c->getScale() - 2.0f) < 1e-4f,
               "with the scale itself unchanged");
        }

        // But a genuinely non-uniform pose still switches the mode, because
        // then the animation really is asking for it.
        {
            ShapeManager sm;
            Cube* c = new Cube(0.0f, 0.0f, 0.0f, 1.0f, 0, 1);
            c->useUniformScaling(false);
            c->setScale(1.0f, 2.0f, 3.0f);
            sm.addShape(c);
            sm.setShapeCounter(2);

            AnimationTimeline tl;
            tl.keyShapeAt(sm, 1, 0.0f);
            tl.keyShapeAt(sm, 1, 1.0f);

            c->useUniformScaling(true);              // user flips it back
            tl.currentTime = 0.5f;
            tl.apply(sm);

            ok(!c->isUsingUniformScaling(),
               "a non-uniform key does switch the mode -- that is the animation "
               "asking, not a side effect of moving the playhead");
        }

        // 2. A deleted shape must not leave an unreachable track behind.
        //
        // Every button in the keyframe section acts on the SELECTED shape, and
        // a deleted shape cannot be selected, so the row was permanent.
        {
            ShapeManager sm;
            Cube* a = new Cube(0.0f, 0.0f, 0.0f, 1.0f, 0, 1);
            Cube* b = new Cube(2.0f, 0.0f, 0.0f, 1.0f, 0, 2);
            sm.addShape(a);
            sm.addShape(b);
            sm.setShapeCounter(3);

            AnimationTimeline tl;
            tl.keyShapeAt(sm, 1, 0.0f);
            tl.keyShapeAt(sm, 2, 0.0f);
            ok(tl.tracks.size() == 2, "two shapes, two tracks");

            ok(tl.pruneOrphanTracks(sm) == 0,
               "nothing is pruned while both shapes exist");

            sm.deleteShape(b);
            ok(tl.pruneOrphanTracks(sm) == 1,
               "deleting a keyed shape leaves exactly one orphan track");
            ok(tl.tracks.size() == 1 && tl.tracks[0].shapeId == 1,
               "and the survivor is the one whose shape is still there");

            ok(tl.pruneOrphanTracks(sm) == 0, "pruning again is a no-op");
        }
    } else {
        std::printf("  (no GL context -- regression checks skipped)\n");
    }

    // =======================================================================
    // EASING
    // =======================================================================
    // Easing changes WHEN the motion happens, never WHERE it ends up. Every
    // check below is about the shape of the curve, not about any pose.
    {
        // The one rule that applies to every curve, including ones a student
        // invents: f(0) = 0 and f(1) = 1. Break it and a key stops holding its
        // own pose at its own time, and the whole track drifts a little further
        // out of place with every segment it crosses.
        bool endpointsHold = true;
        for (int m = 0; m < EASE_COUNT; ++m) {
            if (std::fabs(applyEasing(m, 0.0f) - 0.0f) > 1e-5f) endpointsHold = false;
            if (std::fabs(applyEasing(m, 1.0f) - 1.0f) > 1e-5f) endpointsHold = false;
        }
        ok(endpointsHold,
           "easing: EVERY curve satisfies f(0) = 0 and f(1) = 1 -- otherwise a "
           "key does not hold its own pose at its own time");

        // And every curve is bounded, so an eased pose can never overshoot past
        // the key it is heading for.
        bool bounded = true;
        for (int m = 0; m < EASE_COUNT; ++m) {
            for (int i = 0; i <= 20; ++i) {
                const float e = applyEasing(m, i / 20.0f);
                if (e < -1e-5f || e > 1.0f + 1e-5f) bounded = false;
            }
        }
        ok(bounded, "easing: no curve leaves [0, 1], so nothing overshoots its key");

        nearly(applyEasing(EASE_LINEAR, 0.25f), 0.25f, "easing: linear is the identity");
        nearly(applyEasing(EASE_LINEAR, 0.75f), 0.75f, "easing: linear, again");

        // Step HOLDS. Returning 0 rather than rounding at 0.5 is what makes it
        // a hold rather than a slow jump.
        ok(applyEasing(EASE_STEP, 0.01f) == 0.0f &&
           applyEasing(EASE_STEP, 0.50f) == 0.0f &&
           applyEasing(EASE_STEP, 0.99f) == 0.0f,
           "easing: step holds the first key's pose for the whole interval and "
           "jumps once, at the next key");

        ok(applyEasing(EASE_IN, 0.5f) < 0.45f,
           "easing: ease-in is BEHIND linear at the midpoint -- it accelerates "
           "out of the key");
        ok(applyEasing(EASE_OUT, 0.5f) > 0.55f,
           "easing: ease-out is AHEAD of linear at the midpoint -- it "
           "decelerates into the next key");
        nearly(applyEasing(EASE_IN_OUT, 0.5f), 0.5f,
               "easing: ease-in-out is symmetric, so it crosses the midpoint on time");
        ok(applyEasing(EASE_IN_OUT, 0.25f) < 0.25f &&
           applyEasing(EASE_IN_OUT, 0.75f) > 0.75f,
           "easing: ease-in-out is slow at BOTH ends and quick through the middle");

        // Ease-in and ease-out are the same curve seen from opposite ends.
        // A student who writes one and mirrors it gets this for free; one who
        // writes two unrelated formulas usually does not.
        bool mirrored = true;
        for (int i = 0; i <= 10; ++i) {
            const float u = i / 10.0f;
            if (std::fabs(applyEasing(EASE_IN, u) -
                          (1.0f - applyEasing(EASE_OUT, 1.0f - u))) > 1e-4f) {
                mirrored = false;
            }
        }
        ok(mirrored, "easing: ease-out is ease-in reflected -- in(u) = 1 - out(1 - u)");

        // Monotonic: time never runs backwards inside an interval.
        bool monotonic = true;
        for (int m = 0; m < EASE_COUNT; ++m) {
            if (m == EASE_STEP) continue;          // flat by design
            float prev = -1.0f;
            for (int i = 0; i <= 40; ++i) {
                const float e = applyEasing(m, i / 40.0f);
                if (e < prev - 1e-5f) monotonic = false;
                prev = e;
            }
        }
        ok(monotonic,
           "easing: every curve except step is non-decreasing -- an object "
           "never backs up inside a segment");
    }
    {
        // Easing reaching evaluate(): the remap has to happen there, on the
        // easing of the key the interval LEAVES, or none of the above matters.
        AnimationTrack tr = twoKeyTrack();          // keys at t = 0 and t = 2
        tr.keys[0].easing = EASE_STEP;

        ok(tr.evaluate(1.0f, out), "easing: a stepped track still evaluates");
        nearlyVec(out.position, tr.keys[0].position,
                  "easing: halfway through a STEPPED interval still holds the "
                  "first key's pose -- evaluate() must remap u, not ignore it");

        ok(tr.evaluate(2.0f, out), "easing: and it evaluates at the far key");
        nearlyVec(out.position, tr.keys[1].position,
                  "easing: a stepped interval still ARRIVES at the next key");

        tr.keys[0].easing = EASE_IN_OUT;
        ok(tr.evaluate(0.5f, out), "easing: an eased track evaluates");
        ok(out.position.x < 2.5f,
           "easing: a quarter of the way into an ease-in-out interval, the "
           "shape has moved LESS than a quarter of the distance");
        ok(tr.evaluate(1.0f, out) && std::fabs(out.position.x - 5.0f) < 1e-3f,
           "easing: and it is exactly halfway at the halfway point -- easing "
           "changes the schedule, never the path");

        // The last key's easing governs nothing, and setting it must not
        // disturb anything. The UI greys it out; the code has to agree.
        tr.keys[0].easing = EASE_LINEAR;
        tr.keys[1].easing = EASE_STEP;
        ok(tr.evaluate(1.0f, out) && std::fabs(out.position.x - 5.0f) < 1e-3f,
           "easing: the LAST key's easing is never read -- there is no interval "
           "to its right");
    }

    std::printf("\n%d checks, %d failures\n", checks, failures);
    if (failures == 0) {
        std::printf("ALL TESTS PASSED (0 failures)\n");
        return 0;
    }
    std::printf("TESTS FAILED (%d failures)\n", failures);
    return 1;
}
