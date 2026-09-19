// =============================================================================
// PendulumSystem::evalF  -- the spring-mass force evaluation
// =============================================================================
// Split out of PendulumSystem.cpp deliberately. This is the ONLY part of the
// pendulum/cloth system a student implements; the other ~650 lines of
// PendulumSystem.cpp are VAO setup, spring/wireframe rendering and buffer
// management that are given.
//
// Keeping them in one translation unit would mean a prebuilt solution library
// had to carry all that rendering code too, so every rendering fix would force
// a rebuild and redistribution of the archive. Separated, the swappable unit is
// exactly the student's work and nothing else.
//
// See SOLUTION-LIBRARIES.md for how this file is swapped for a prebuilt
// solution at link time.

#include "PendulumSystem.h"
#include <cmath>

std::vector<glm::vec3> PendulumSystem::evalF(const std::vector<glm::vec3>& state) {
    // NOT PART OF THIS COURSE -- this is particles material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // State is interleaved [pos0, vel0, pos1, vel1, ...]. For each particle
    // return [velocity, force / mass]. Forces are gravity, viscous drag
    // (-k * velocity) and, for every spring touching this particle, Hooke:
    //     -k * (|d| - restLength) * d / |d|
    // Fixed particles contribute zero so they stay put.
    //
    // Return the same size as `state`, zero-filled, so an unfinished
    // implementation leaves the system still rather than crashing the
    // integrator on a size mismatch.
    return std::vector<glm::vec3>(state.size(), glm::vec3(0.0f));
}
