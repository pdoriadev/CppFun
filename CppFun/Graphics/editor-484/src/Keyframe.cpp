// =============================================================================
// Keyframe storage and evaluation -- the whole of Assignment 4
// =============================================================================
// Split out of AnimationTimeline.cpp deliberately. That file keeps what is
// always given: the timeline itself, applying a pose to a shape, playback and
// scene IO. This file holds the student's work, which is two things that belong
// together:
//
//   STORAGE       addKey, removeKeyNear, moveKey -- maintain the invariant
//                 that keys are sorted by time and unique in it
//   EVALUATION    applyEasing, lerpVec3, lerpEulerDegrees and evaluate()
//                 -- depend on that invariant
//
// They are in one file on purpose. A bug in the first half surfaces in the
// second: two keys at one instant divide by zero inside evaluate(), and an
// unsorted vector breaks its bracketing search. Owning both is what makes that
// debuggable rather than mystifying.
//
// The split is also what makes a per-topic solution library possible: the unit
// the linker can swap is a whole translation unit, so the swappable code has to
// live in a file of its own. See SOLUTION-LIBRARIES.md.

#include "Keyframe.h"

#include <algorithm>
#include <cmath>

// Given: the labels, so the UI and the scene file agree on spelling.
const char* const kEasingNames[EASE_COUNT] = {
    "Step", "Linear", "Ease In", "Ease Out", "Ease In-Out"
};


// Linear interpolation with u clamped, so a caller that computes u from an
// out-of-range t cannot extrapolate a pose that was never keyed.
glm::vec3 lerpVec3(const glm::vec3& a, const glm::vec3& b, float u) {
    // TODO(animation): linear interpolation, with u clamped to [0, 1]
    (void)u;
    return a + (b - a) * 0.0f;
}

// Interpolate Euler angles the short way round.
//
// Wrapping the per-axis difference into (-180, 180] is the whole trick. Going
// from 350 to 10 degrees, b - a is -340; adding 360 gives +20, which is the
// rotation a viewer expects to see. Without this an object spins almost all
// the way round the wrong way whenever a track crosses the 0/360 seam.
glm::vec3 lerpEulerDegrees(const glm::vec3& a, const glm::vec3& b, float u) {
    // TODO(animation): interpolate Euler angles the SHORT way round
    // Per axis: wrap the difference b - a into (-180, 180] before applying
    // it, or a track crossing 350 -> 10 degrees spins backwards through
    // the whole circle instead of forwards through 20.
    (void)b; (void)u;
    return a;
}

// Remap u through an easing curve. See Keyframe.h for the contract; the one
// rule that matters is f(0) = 0 and f(1) = 1, so a key still holds its own pose
// at its own time.
float applyEasing(int easing, float u) {
    if (u <= 0.0f) return 0.0f;
    if (u >= 1.0f) return 1.0f;

    // TODO(animation): remap u through the easing curve
    // Return u remapped by the curve `easing` names. The enum is in
    // Keyframe.h; every curve must satisfy f(0) = 0 and f(1) = 1, or a key
    // stops holding its own pose at its own time.
    //
    //   EASE_LINEAR    u
    //   EASE_IN        u^2                 accelerates out of the key
    //   EASE_OUT       1 - (1 - u)^2       decelerates into the next
    //   EASE_IN_OUT    3u^2 - 2u^3         slow at both ends (smoothstep)
    //   EASE_STEP      0                   hold, then jump at the next key
    //
    // Leaving this as-is makes every curve linear: the picker in the panel
    // works, the scene file remembers your choice, and nothing looks any
    // different. That is the checkpoint -- the motion is correct, just
    // mechanical.
    (void)easing;
    return u;
}

// --- storage: keeping the keys sorted and unique in time --------------------

void AnimationTrack::addKey(const Keyframe& k) {
    // TODO(animation): insert keeping keys sorted, replacing any key at the same time
    // Insert k so that `keys` stays sorted by time, and so that no two keys
    // ever share an instant: a key at a time that already has one REPLACES
    // it rather than joining it.
    //
    // That rule is not tidiness. Two keys at one time make evaluate() depend
    // on insertion order, and make its u = (t - a.time) / (b.time - a.time)
    // divide by zero. You are about to write that code, or you already have.
    //
    // As it stands this appends without sorting or replacing. The timeline
    // looks entirely alive -- keys land, tracks appear, diamonds drag -- and
    // it misbehaves the moment you key out of order or key the same instant
    // twice. Both are visible, and both are yours to fix.
    keys.push_back(k);
}

bool AnimationTrack::removeKeyNear(float t, float eps) {
    // TODO(animation): erase the key NEAREST t, if one is within eps
    // Erase the key NEAREST t, provided it is within eps, and say whether
    // one went. Nearest, not first: the playhead rarely lands exactly on a
    // key, and Delete key passes a generous eps of 0.05 s precisely so it
    // can find the one you meant.
    //
    // As it stands this takes the first key within eps instead, which is the
    // wrong one whenever two keys sit close together.
    for (size_t i = 0; i < keys.size(); ++i) {
        if (std::fabs(keys[i].time - t) <= eps) {
            keys.erase(keys.begin() + static_cast<long>(i));
            return true;
        }
    }
    return false;
}

int AnimationTrack::moveKey(size_t index, float t, float eps) {
    // TODO(animation): retime the key, keep the list sorted, return where it ended up
    // Retime the key at `index` to time t, keep `keys` sorted, and RETURN
    // THE INDEX IT ENDED UP AT. Return -1 if `index` was out of range.
    // Clamp t at 0; there is no negative time.
    //
    // Three things will bite you, in the order you will meet them:
    //
    //   1. Returning the new index is the entire point. Dragging a key past
    //      its neighbour re-sorts the vector underneath the drag, and the
    //      caller in Renderer.cpp stores what you return so it keeps hold of
    //      the SAME keyframe. Return the old index and the drag silently
    //      jumps to a different key halfway through the gesture.
    //   2. Lift the key out BEFORE looking for collisions, or the sweep
    //      matches the key against itself and deletes the thing being moved.
    //   3. Dropping onto an existing key replaces it -- the moved key wins
    //      and the list gets shorter. Same rule as addKey, same reason. If
    //      you erase while iterating, only advance the index when you did
    //      not erase.
    //
    // As it stands this retimes in place and returns the index it was given.
    // Dragging works perfectly until a key crosses one of its neighbours.
    if (index >= keys.size()) return -1;
    if (t < 0.0f) t = 0.0f;
    keys[index].time = t;
    (void)eps;
    return static_cast<int>(index);
}

bool AnimationTrack::evaluate(float t, Keyframe& out) const {
    // TODO(animation): return the interpolated pose at time t
    // Handle, in order: no keys (return false); exactly one key; looping
    // (wrap t into [startTime, endTime]); t at or outside either end
    // (clamp, never extrapolate); otherwise find the bracketing pair,
    // compute u across the interval, remap it through
    // applyEasing(k[i].easing, u), and interpolate. See the full contract
    // in include/Keyframe.h.
    (void)t;
    return false;
}
