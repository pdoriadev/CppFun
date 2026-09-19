#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <vector>
#include <glm/glm.hpp>

class ShapeManager;

// =============================================================================
// Keyframe animation
// =============================================================================
// A track is a list of poses in time for one shape. Playback asks the track
// "what is the pose at t?" and the track interpolates between the two keys
// that bracket t.
//
// This is deliberately separate from the particle systems. Those integrate a
// differential equation forward and cannot be asked about an arbitrary time;
// a keyframe track is a pure function of t, which is why scrubbing a timeline
// backwards works here and does not there.
//
// FILE SPLIT -- Keyframe.cpp holds only AnimationTrack::evaluate() and the
// interpolation helpers it uses. Everything else (storage, insertion,
// applying a pose to a shape, playback, scene IO) lives in
// AnimationTimeline.cpp. The unit a solution library can swap is a whole
// translation unit, so the swappable code has to be a file of its own.
// See SOLUTION-LIBRARIES.md and ASSIGNMENTS.md.

// How the interval LEAVING a key is timed.
//
// Stored on the key the segment starts from, which is the convention every
// animation tool uses: key i's easing governs the stretch between key i and
// key i + 1. The last key's easing is therefore never read, and that is not a
// bug -- there is no interval to its right.
//
// Easing changes only WHEN the motion happens, never WHERE it goes. u still
// runs 0 to 1 across the interval and still ends where the next key is; the
// curve decides how fast it gets there. That separation is why a single
// remapping function can serve position, rotation and scale at once.
enum Easing {
    EASE_STEP = 0,   // hold the first key's pose, then jump. No interpolation.
    EASE_LINEAR,     // u unchanged -- constant speed, and visibly mechanical
    EASE_IN,         // start slow, end fast
    EASE_OUT,        // start fast, end slow
    EASE_IN_OUT,     // slow at both ends -- what most motion actually looks like
    EASE_COUNT
};

// Names for the UI and the scene file, indexed by the enum above.
extern const char* const kEasingNames[EASE_COUNT];

// One pose at one instant. Rotation is Euler degrees about X, Y, Z, matching
// Shape's angleX/angleY/angleZ so no conversion is needed at either end.
struct Keyframe {
    float     time;
    glm::vec3 position;
    glm::vec3 rotation;      // degrees
    glm::vec3 scale;
    int       easing;        // one of Easing; governs the interval to the RIGHT

    Keyframe()
        : time(0.0f),
          position(0.0f),
          rotation(0.0f),
          scale(1.0f),
          easing(EASE_LINEAR) {}

    // Easing is deliberately not a constructor parameter. Every existing caller
    // builds a key from a pose, and defaulting to linear keeps them all correct
    // without a fifth argument threaded through code that does not care.
    Keyframe(float t, const glm::vec3& p, const glm::vec3& r, const glm::vec3& s)
        : time(t), position(p), rotation(r), scale(s), easing(EASE_LINEAR) {}
};

// --- interpolation helpers (student work, in Keyframe.cpp) -------------------

// Straight linear interpolation, u clamped to [0, 1].
glm::vec3 lerpVec3(const glm::vec3& a, const glm::vec3& b, float u);

// Interpolate Euler angles the short way round.
//
// Naive lerp from 350 to 10 degrees sweeps 340 degrees backwards through the
// whole circle instead of 20 degrees forwards. Each axis is interpolated along
// whichever direction is shorter, by wrapping the difference into
// (-180, 180] before applying it.
glm::vec3 lerpEulerDegrees(const glm::vec3& a, const glm::vec3& b, float u);

// WHERE THIS STOPS, AND WHAT COMES AFTER IT.
//
// Interpolating each axis separately -- even taking the short way round on each
// one -- is not the same as taking the short way round in 3D. Three independent
// angle sweeps trace a path through orientation space that depends on the axis
// order, wanders off the direct route, and changes angular speed along the way.
// Euler angles also cannot represent every rotation cleanly: at pitch +/-90
// degrees two axes line up, a degree of freedom is lost, and the
// representation is stuck. That is gimbal lock, and it is a property of the
// three-angle representation itself, not of the interpolation.
//
// The standard fix is to store rotations as unit quaternions and interpolate
// along the arc between them (spherical linear interpolation, "slerp"), which
// gives a single rotation about a single axis at constant angular velocity.
// Every production animation system does this.
//
// It is deliberately NOT part of this course. CPSC 484 is a principles course,
// and the quaternion machinery -- half-angle construction, the double cover,
// the slerp derivation, extracting Euler angles back out -- is a lot of
// apparatus for one function. Knowing that the limitation exists, and being
// able to see it on screen, is the outcome worth having here. Chasing it is a
// good final-project topic and standard material in a later course.

// Remap u through an easing curve. Returns the eased parameter, still in
// [0, 1], which the caller then feeds to the three interpolators.
//
// Every curve must satisfy f(0) = 0 and f(1) = 1 -- otherwise a key does not
// hold its own pose at its own time, and the animation drifts a little further
// off with every segment. EASE_STEP is the deliberate exception at the near
// end: it returns 0 for everything below 1.
float applyEasing(int easing, float u);

// --- the track --------------------------------------------------------------

struct AnimationTrack {
    int                   shapeId;
    std::vector<Keyframe> keys;      // kept sorted by time; see addKey()
    bool                  loop;

    AnimationTrack() : shapeId(-1), loop(false) {}
    explicit AnimationTrack(int id) : shapeId(id), loop(false) {}

    // --- student work: storage, in Keyframe.cpp ---
    //
    // These three and evaluate() are two halves of ONE invariant: the keys are
    // sorted by time and no two share an instant. These maintain it; evaluate()
    // depends on it. Getting addKey() wrong does not show up in addKey() -- it
    // shows up as a division by zero or an out-of-order bracket inside the
    // interpolation you also wrote, which is exactly the debugging this
    // assignment is for.

    // Inserts keeping keys sorted by time. A key at an existing time replaces
    // it rather than producing two keys at the same instant, which would make
    // evaluate() depend on insertion order.
    void addKey(const Keyframe& k);

    // Removes the key nearest t if it is within eps. Returns whether one went.
    bool removeKeyNear(float t, float eps = 1e-4f);

    // Retimes the key at `index` to time t, keeping the list sorted, and
    // returns the index it ENDED UP at -- or -1 if `index` was out of range.
    //
    // Returning the new index is the whole point. Dragging a key past its
    // neighbour reorders the vector underneath the drag, so a caller holding an
    // index would silently start dragging a different key mid-gesture. The
    // caller stores what comes back and keeps hold of the same keyframe.
    //
    // Dropping onto an existing key REPLACES it: the moved key wins and the
    // list gets shorter. The alternative -- two keys at one instant -- makes
    // evaluate() depend on insertion order and divides by zero interpolating
    // between them, which is the same reason addKey() replaces rather than
    // appends.
    //
    // t is clamped at 0; there is no negative time.
    int moveKey(size_t index, float t, float eps = 1e-4f);

    // --- given: derived from the keys, in AnimationTimeline.cpp ---

    float startTime() const;
    float endTime() const;
    float duration() const;          // endTime - startTime, 0 if fewer than 2 keys

    // --- student work: evaluation, in Keyframe.cpp ---

    // Writes the pose at time t into out and returns true, or returns false if
    // the track has no keys at all.
    //
    // Contract, in the order the cases must be handled:
    //
    //   no keys              return false, leave out untouched
    //   exactly one key      that key's pose, whatever t is
    //   loop and t outside   wrap t into [startTime, endTime] first, then
    //                        proceed as below
    //   t <= startTime       the first key's pose (clamp; no extrapolation)
    //   t >= endTime         the last key's pose  (clamp; no extrapolation)
    //   otherwise            find the keys i and i+1 that bracket t and
    //                        interpolate with u = (t - k[i].time)
    //                                            / (k[i+1].time - k[i].time)
    //
    // u is then remapped through applyEasing(k[i].easing, u) -- the easing of
    // the key the interval LEAVES -- before any interpolation happens.
    //
    // position and scale interpolate with lerpVec3, rotation with
    // lerpEulerDegrees. out.time is set to the t actually evaluated (after
    // wrapping, if the track loops), and out.easing carries k[i]'s, so a caller
    // can tell which segment the pose came from.
    bool evaluate(float t, Keyframe& out) const;
};

// --- the timeline (all given, AnimationTimeline.cpp) -------------------------

struct AnimationTimeline {
    std::vector<AnimationTrack> tracks;
    float  currentTime;
    bool   playing;
    float  speed;
    bool   loopAll;

    // How long the timeline IS, as opposed to how much of it currently has
    // keys on it. These must be separate values.
    //
    // Deriving the scrubbable range from duration() -- the extent of the keys
    // -- is a chicken and egg: you cannot move the playhead past your last key,
    // so you can never place a key beyond it. The timeline froze at whatever
    // you keyed first. With no keys at all there was nothing to derive from
    // and the range collapsed to a hardcoded one second.
    //
    // So `length` is what the slider spans and what playback loops over, and
    // it grows automatically when a key is placed beyond it. duration() stays
    // as the extent of the content, which is what the UI reports and what the
    // per-track clamping uses.
    float  length;

    AnimationTimeline()
        : currentTime(0.0f), playing(false), speed(1.0f), loopAll(true),
          length(kDefaultLength) {}

    // Five seconds: long enough to key a few poses without immediately
    // widening it, short enough that the slider has usable resolution.
    static const float kDefaultLength;

    void clear();

    AnimationTrack*       findTrack(int shapeId);
    const AnimationTrack* findTrack(int shapeId) const;
    AnimationTrack&       trackFor(int shapeId);     // creates if absent
    void                  removeTrack(int shapeId);

    // Captures a shape's current transform as a key at time t.
    void keyShapeAt(const ShapeManager& sm, int shapeId, float t);

    // Extent of the keys -- the last keyed time across every track. This is
    // what the panel reports as the content length. It is NOT the scrub range;
    // see `length` above.
    float duration() const;

    // Grows `length` so t fits, never shrinks it. Called when a key is placed.
    void ensureLength(float t);

    // Evaluates every track at currentTime and writes the poses onto the
    // shapes. Shapes without a track are left alone.
    void apply(ShapeManager& sm) const;

    // Advances currentTime by dt * speed and applies. Call once per frame.
    void update(float dt, ShapeManager& sm);

    void reset(ShapeManager& sm);

    // Nearest key strictly before (forward = false) or after (forward = true)
    // time t, written to `out`; returns false if there is none in that
    // direction. `only` restricts the search to one track; passing 0 searches
    // every track.
    //
    // Lives here rather than in the panel that drives the Key< / Key> buttons
    // because it is a question about the timeline, not about the UI -- and
    // because logic buried in an ImGui draw call cannot be tested. The
    // all-tracks fallback is what keeps those buttons useful with nothing
    // selected.
    bool nearestKey(const AnimationTrack* only, float t, bool forward,
                    float& out) const;

    // Drops every track whose shape no longer exists. Returns how many went.
    //
    // A track is keyed by shape id, and deleting a shape does not touch the
    // timeline, so the track outlives it: a row labelled "(deleted 2)" that
    // apply() skips, that no button can reach -- every one of them acts on the
    // SELECTED shape, and a deleted shape cannot be selected -- and that the
    // scene writer happily saves. Called once a frame rather than from the
    // delete handler, so it cannot be missed by a deletion path that does not
    // exist yet.
    int pruneOrphanTracks(const ShapeManager& sm);
};

extern AnimationTimeline g_timeline;

#endif // KEYFRAME_H
