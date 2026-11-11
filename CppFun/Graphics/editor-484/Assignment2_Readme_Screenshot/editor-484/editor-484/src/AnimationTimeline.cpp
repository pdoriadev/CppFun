// =============================================================================
// AnimationTimeline -- track storage and playback
// =============================================================================
// Everything here is given to the student. The interpolation that answers
// "what is the pose at t?" lives in Keyframe.cpp, which is the assignment.

#include "Keyframe.h"
#include "Shape.h"
#include "ShapeManager.h"

#include <algorithm>
#include <cmath>

AnimationTimeline g_timeline;

const float AnimationTimeline::kDefaultLength = 5.0f;

// --- AnimationTrack: derived from the keys -----------------------------------
//
// addKey(), removeKeyNear() and moveKey() are NOT here -- they live in
// Keyframe.cpp, the swappable unit for the `animation` topic, alongside
// evaluate(). They belong with it: those three maintain the invariant that the
// keys are sorted and unique in time, and evaluate() is what breaks when that
// invariant does not hold. Splitting them across two files would put the bug
// and its symptom in different halves of the assignment.
//
// The three below stay given. They read the keys and derive nothing the student
// has to get right.

float AnimationTrack::startTime() const {
    return keys.empty() ? 0.0f : keys.front().time;
}

float AnimationTrack::endTime() const {
    return keys.empty() ? 0.0f : keys.back().time;
}

float AnimationTrack::duration() const {
    return (keys.size() < 2) ? 0.0f : (keys.back().time - keys.front().time);
}

// --- AnimationTimeline ------------------------------------------------------

void AnimationTimeline::clear() {
    tracks.clear();
    currentTime = 0.0f;
    playing     = false;
    speed       = 1.0f;
    loopAll     = true;
    length      = kDefaultLength;
}

void AnimationTimeline::ensureLength(float t) {
    if (t > length) length = t;
}

AnimationTrack* AnimationTimeline::findTrack(int shapeId) {
    for (size_t i = 0; i < tracks.size(); ++i) {
        if (tracks[i].shapeId == shapeId) return &tracks[i];
    }
    return nullptr;
}

const AnimationTrack* AnimationTimeline::findTrack(int shapeId) const {
    for (size_t i = 0; i < tracks.size(); ++i) {
        if (tracks[i].shapeId == shapeId) return &tracks[i];
    }
    return nullptr;
}

AnimationTrack& AnimationTimeline::trackFor(int shapeId) {
    if (AnimationTrack* t = findTrack(shapeId)) return *t;
    tracks.push_back(AnimationTrack(shapeId));
    return tracks.back();
}

void AnimationTimeline::removeTrack(int shapeId) {
    for (size_t i = 0; i < tracks.size(); ++i) {
        if (tracks[i].shapeId == shapeId) {
            tracks.erase(tracks.begin() + static_cast<long>(i));
            return;
        }
    }
}

void AnimationTimeline::keyShapeAt(const ShapeManager& sm, int shapeId, float t) {
    const Shape* s = sm.getShapeById(shapeId);
    if (!s) return;

    // Record the non-uniform scale in every case: a shape keyed while in
    // uniform mode and then switched would otherwise animate towards a scale
    // it never had.
    const glm::vec3 scl = s->isUsingUniformScaling()
                        ? glm::vec3(s->getScale())
                        : s->getNonUniformScale();

    // Keying past the end widens the timeline rather than being refused. The
    // alternative -- clamping the key back to `length` -- silently puts it
    // somewhere the user did not ask for.
    ensureLength(t);

    trackFor(shapeId).addKey(Keyframe(
        t,
        glm::vec3(s->getX(), s->getY(), s->getZ()),
        glm::vec3(s->getAngleX(), s->getAngleY(), s->getAngleZ()),
        scl));
}

float AnimationTimeline::duration() const {
    float d = 0.0f;
    for (size_t i = 0; i < tracks.size(); ++i) {
        d = std::max(d, tracks[i].endTime());
    }
    return d;
}

void AnimationTimeline::apply(ShapeManager& sm) const {
    Keyframe pose;
    for (size_t i = 0; i < tracks.size(); ++i) {
        const AnimationTrack& tr = tracks[i];
        if (!tr.evaluate(currentTime, pose)) continue;   // empty track

        Shape* s = sm.getShapeById(tr.shapeId);
        if (!s) continue;                                // shape was deleted

        s->setPosition(pose.position.x, pose.position.y, pose.position.z);
        s->setRotation(pose.rotation.x, pose.rotation.y, pose.rotation.z);

        // Scale, WITHOUT dragging the shape out of uniform mode.
        //
        // This used to call useUniformScaling(false) unconditionally, on the
        // reasoning that driving one path is simpler than driving two. The cost
        // was invisible from here and obvious in the panel: scrubbing the
        // timeline silently unticked "Uniform scaling" on every keyed shape,
        // because apply() runs on every frame of a scrub and the flag is user
        // state, not animation state.
        //
        // A key captured from a uniformly-scaled shape holds three equal
        // numbers, so the uniform path reproduces it exactly. Only a genuinely
        // non-uniform pose needs the mode switched, and then the switch is
        // something the animation really is asking for rather than a side
        // effect of moving the playhead.
        const float e = 1e-5f;
        const bool uniformPose =
            std::fabs(pose.scale.x - pose.scale.y) < e &&
            std::fabs(pose.scale.y - pose.scale.z) < e;

        if (uniformPose && s->isUsingUniformScaling()) {
            s->setScale(pose.scale.x);
        } else {
            s->useUniformScaling(false);
            s->setScale(pose.scale.x, pose.scale.y, pose.scale.z);
        }
    }
}

void AnimationTimeline::update(float dt, ShapeManager& sm) {
    if (!playing) return;

    currentTime += dt * speed;

    // Loop over the timeline's LENGTH, not the extent of its keys. A shape
    // whose last key is at t=2 on a 5-second timeline should hold that pose
    // for three seconds and then loop, which is what every animation tool
    // does. Looping on duration() would cut the timeline short at the last
    // key instead.
    const float end = length;
    if (loopAll && end > 0.0f) {
        if (currentTime > end) {
            currentTime = std::fmod(currentTime, end);
        } else if (currentTime < 0.0f) {
            // Negative speed scrubs backwards; wrap round to the end.
            currentTime = end + std::fmod(currentTime, end);
        }
    } else if (currentTime > end) {
        currentTime = end;
        playing     = false;
    } else if (currentTime < 0.0f) {
        currentTime = 0.0f;
        playing     = false;
    }

    apply(sm);
}

int AnimationTimeline::pruneOrphanTracks(const ShapeManager& sm) {
    int removed = 0;
    for (size_t i = 0; i < tracks.size(); ) {
        if (sm.getShapeById(tracks[i].shapeId) == 0) {
            tracks.erase(tracks.begin() + i);
            ++removed;
        } else {
            ++i;
        }
    }
    return removed;
}

void AnimationTimeline::reset(ShapeManager& sm) {
    currentTime = 0.0f;
    playing     = false;
    apply(sm);
}

bool AnimationTimeline::nearestKey(const AnimationTrack* only, float t,
                                   bool forward, float& out) const {
    bool  found = false;
    float best  = 0.0f;

    // Strictly before / after, with a tolerance: the playhead usually sits
    // exactly ON a key after a jump, and without the epsilon a second press
    // would find that same key again and the button would appear dead.
    const float eps = 1e-4f;

    for (size_t i = 0; i < tracks.size(); ++i) {
        const AnimationTrack& tr = tracks[i];
        if (only && &tr != only) continue;

        for (size_t k = 0; k < tr.keys.size(); ++k) {
            const float kt = tr.keys[k].time;
            if (forward) {
                if (kt <= t + eps) continue;
                if (!found || kt < best) { best = kt; found = true; }
            } else {
                if (kt >= t - eps) continue;
                if (!found || kt > best) { best = kt; found = true; }
            }
        }
    }

    out = best;
    return found;
}
