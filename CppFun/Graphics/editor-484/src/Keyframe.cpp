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

#include "../include/Keyframe.h"

#include <algorithm>
#include <cmath>
#include <iostream>

// Given: the labels, so the UI and the scene file agree on spelling.
const char* const kEasingNames[EASE_COUNT] = {
    "Step", "Linear", "Ease In", "Ease Out", "Ease In-Out"
};

/*
TO-DO 
    - epsilon when replacing key on add. Should remove within a range. Can have keys with 0.01 difference right now. No bueno. 
    - not currently rotating the opposite direction like it should. ): take a nother look. Take to prof. 
        - not setting negative rotation values. related?
        - run through math. May need to flip at the end??
*/


// SHOULD THIS BE IN A GENERAL USE MATH LIBRARY FOR THE ENGINE???
// Linear interpolation with u clamped, so a caller that computes u from an
// out-of-range t cannot extrapolate a pose that was never keyed.
glm::vec3 lerpVec3(const glm::vec3& a, const glm::vec3& b, float u) {
    if      (u < 0 )    u = 0;
    else if (u > 1)     u = 1;
    return a + (b - a) * u;
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
    if      (u < 0 )    u = 0;
    else if (u > 1)     u = 1;

    glm::vec3 lerped = glm::vec3(INFINITY, INFINITY, INFINITY);
    for (uint8_t i = 0; i < 3; ++i) {
        if (a[i] == b[i]) { 
            lerped[i] = a[i]; 
            continue;
        }

        float aangle = a[i];
        float bangle = b[i];
        {   // wraps angles to a [0 , 360) domain.  
            int const aRotations = ((int)aangle / 360); 
            int const bRotations = ((int)bangle / 360);

            aangle -= (float)(aRotations /*+ aNegOffset*/) * 360.0f;
            bangle -= (float)(bRotations /*+ bNegOffset*/) * 360.0f;
        }

        // angle difference. Adjust to be within [-180, 180) domain.
        float diffAngle = bangle - aangle;
        if      (diffAngle >= 180) diffAngle -= 360.0f;
        else if (diffAngle < -180) diffAngle += 360.0f; 
 
        lerped[i] = a[i] + diffAngle * u;
    }

    // LOGIC ERROR CHECK
    for (uint8_t i = 0; i < 3; ++i) {
        if (lerped[i] == INFINITY) {
            std::cerr << "Failed to set new angle value for index " << i << std::endl; 
        }
    }

    return lerped;
}

// Remap u through an easing curve. See Keyframe.h for the contract; the one
// rule that matters is f(0) = 0 and f(1) = 1, so a key still holds its own pose
// at its own time.
// u is not for a whole track, but *between* two keys. 
float applyEasing(EasingTypes const easingType, float const u) {
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
    
    if (u <= 0.0f) return 0.0f;
    if (u >= 1.0f) return 1.0f;
    float easedU = -1.0f;
    switch(easingType) {
        case EASE_STEP :
            if (u < 1.0f) easedU = 0.0f;
            easedU = 1.0f;
            break;
        case EASE_LINEAR :
            easedU = u;
            break;
        case EASE_IN :
            easedU = u*u;
            break;
        case EASE_OUT :
            easedU = 1 - ( (1-u) * (1-u) );
            break;
        case EASE_IN_OUT : // smoothstep
            easedU = 3*u*u - 2*u*u*u;
            break;
        default:
            std::cerr << easingType << " is not a valid easing type." << std::endl;
    }

    if (easedU == -1.0f) { std::cerr << "Failed to apply an easing type. u = " << u << std::endl; }

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

    if (keys.size() == 0) { keys.push_back(k); }
    
    // insert at end. 
    if (k.time > keys.at(keys.size() - 1).time) {
        keys.push_back(k);
        return;
    }
    
    // insert anywhere else. shift or replace.
    for (uint i = 0; i < keys.size(); ++i) {
        if (k.time > keys.at(i).time) {
            continue;
        }

        if (k.time == keys.at(i).time) {
            keys[i] = k;
            return;
        }
        
        // k.time < keys[i].time
        keys.insert(keys.begin() + i, k);
        return;
    }
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
    if (t < 0.0f) t = 0.0f;

    int closest = -1;
    float closestDistSquared = INFINITY;
    for (size_t i = 0; i < keys.size(); ++i) {
        float const distanceSquared = (keys[i].time - t) * (keys[i].time - t);
        if (distanceSquared * distanceSquared > eps * eps) {
            if (closest != -1) break; // outside epsilon's range
            continue; 
        }

        if (distanceSquared < closestDistSquared) {
            closest = i;
            closestDistSquared = distanceSquared;
            continue;
        }  
    }

    if (closest == -1) return false;
    
    // keys are sorted. 
    keys.erase(keys.begin() + closest);
    return true;  
}

int AnimationTrack::moveKey(size_t const K_INDEX, float t, float const EPS) {
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

    if (K_INDEX >= keys.size()) return -1; // invalid index.
    if (t < 0.0f) t = 0.0f; // clamp t. 
    if (t == keys.at(K_INDEX).time) return K_INDEX; // key is already at correct index

    Keyframe const K = keys.at(K_INDEX);
        
    // CASES for t
        // t < firstKey.time - insertionIndex = 0 -------------
        // t < keys[i].time - insertionIndex = i **************
        // t > lastKey.time - insertionIndex = LAST_INDEX *****
        // t > keys[i].time - YOU MESSED UP. xxxxxxxxxxxxxxxxxx
    int i = 0;
    size_t const LAST_INDEX = keys.size() - 1;
    // Find insertionIndex value.
    for ( ; i <= LAST_INDEX; ++i) {
        if (t <= keys.at(i).time) { break; }
    }
    
    if (i > LAST_INDEX + 1) { std::cerr << "moveKey() i " << i << " is greater than key size " << std::endl; return -1; }
    
    // Set i, removal index, insertions. 
    // CASES for insertionIndex
        // UPDATE END       - i == LAST_INDEX + 1 && index == LAST_INDEX - update end value
        // NEW END          - i == LAST_INDEX + 1 && index != LAST_INDEX 
        // UPDATE NON-END   - index = insertionIndex - update time value.
        // INSERT NON-END   - index < insertionIndex - insert at insertionIndex. 
        // index > insertionIndex - insert. 

    // std::string actionsLog = "";

    // NOTE: The extended if-else block smells like I didn't constrain the 
        // state space enough before-hand. 

    int removalIndex = -1; // assume overwrite until not. 
    // resolve OOB cases first. overwrite.
    if (i == LAST_INDEX + 1 && LAST_INDEX == K_INDEX) { 
        --i;
        // actionsLog += "Moved key is last key. Overwrote last key's t.";
    } 
    // resolve OOB cases first. add new end. 
    else if (i == LAST_INDEX + 1) { 
        keys.push_back(K);
        removalIndex = K_INDEX;
        // actionsLog += "Added copy of moved key to the end.";
    } 
    // Update time. Overwrite. Moved a little left.
    else if (i == K_INDEX) { 
        // actionsLog += "Moved key left. Overwrote moving key's t value.";
    }
    // Update time. Overwrite. Moved a little right. 
    else if (i == K_INDEX + 1) {
        // actionsLog += "Moved key right. Overwrote moving key's t value.";
        --i;
    }
    // t overlaps with another key's time. overwrite.
    else if (t == keys.at(i).time) {
         keys[i] = K;
         removalIndex = K_INDEX; 
         // actionsLog += "Moved key's new t is equal to key at index " + std::to_string(i) + ". Overwrite.";
    }
    // insert before i
    else {
        keys.insert(keys.begin() + i, K); 
        if (i < K_INDEX) { removalIndex = K_INDEX + 1; }
        else removalIndex = K_INDEX;
        // actionsLog += "Inserted copy of moved key to index " + std::to_string(i - 1) + ".";
    } 

    keys.at(i).time = t;

    if (removalIndex >= 0) {
        keys.erase(keys.begin() + removalIndex); 
        if (i > removalIndex) --i;
        // actionsLog += " Removed key at index " + std::to_string(removalIndex);
    }

    /* std::cout << "Key old index: " << K_INDEX 
                << ". New Index: " << i 
                << ". New t = " << t << std::endl; */
    // std::cout << actionsLog << std::endl;

    return i;
}

bool AnimationTrack::evaluate(float t, Keyframe& out) const {
    // TODO(animation): return the interpolated pose at time t
    // Handle, in order: no keys (return false); exactly one key; looping
    // (wrap t into [startTime, endTime]); t at or outside either end
    // (clamp, never extrapolate); otherwise find the bracketing pair,
    // compute u across the interval, remap it through
    // applyEasing(k[i].easing, u), and interpolate. See the full contract
    // in include/Keyframe.h.
    if (keys.size() == 0) return false;
    if (keys.size() == 1) {
        out.time = keys.at(0).time;
        out.easing = keys.at(0).easing; 
        return false;
    }

    // Clamp 
    if (t < 0.0f) t = 0.0f;
    float const START_TIME = keys.at(0).time;
    float const END_TIME = keys.at(keys.size() - 1).time;
    
    // WRAP LOOP
    if (loop) {
        uint const LOOPS = uint(t / END_TIME);
        t -= END_TIME * (float)LOOPS;
    }

    // LAST KEY CASE 
    if (t >= END_TIME) {
        size_t const INDEX = keys.size() - 1;
        easeKeyframe(EaseKeyframeParams(keys.at(INDEX-1), keys.at(INDEX), 1.0f, out));
        return true;
    }    

    // FIRST KEY CASE
    if (t <= START_TIME) {
        easeKeyframe(EaseKeyframeParams(keys.at(0), keys.at(1), 0.0f, out));
        return true;
    }

    // IN-BETWEEN CASE
    int aIndex = 0;
    int bIndex = -1;
    for (; aIndex < keys.size(); ++aIndex) {
        float const iTime = keys.at(aIndex).time;
        if (t > iTime) { continue; }

        if (t == iTime) { 
            bIndex = aIndex + 1;
            break;
        }

        if (t < iTime) {
            bIndex = aIndex;
            --aIndex;
            break;
        }
    }
    
    if (bIndex < 0) { std::cerr << "bIndex never assigned" << std::endl; }

    Keyframe const & a = keys.at(aIndex);
    Keyframe const & b = keys.at(bIndex);
    if (a.time == b.time) {
        std::cerr << "Keys have the same time value! a = " << a.time << ". b = " << b.time << std::endl;  
        out.time = a.time;
        out.easing = a.easing;
        return true;
    }

    float u = (t - a.time) / (b.time - a.time);
    easeKeyframe(EaseKeyframeParams(a, b, u, out));

    return true;
}

bool AnimationTrack::easeKeyframe(EaseKeyframeParams p) const {
    p.out.time     = p.a.time + p.u;
    p.out.easing   = p.a.easing;
    p.out.position = lerpVec3(p.a.position, p.b.position, p.u);
    p.out.rotation = lerpEulerDegrees(p.a.rotation, p.b.rotation, p.u);
    p.out.scale    = lerpVec3(p.a.scale, p.b.scale, p.u);

    return true;
}
