#include "AnimationController.h"

#include "ShapeManager.h"
#include "ParticleSystem.h"

#include <cmath>
#include <sstream>
#include <vector>

AnimationController g_animation;

namespace {

// True if any particle has gone non-finite or flown implausibly far. Both are
// symptoms of the same thing: the integrator has become unstable for the given
// step size.
bool hasDiverged(const std::vector<ParticleSystem*>& systems, float limit) {
    for (size_t i = 0; i < systems.size(); ++i) {
        if (!systems[i]) continue;

        std::vector<glm::vec3> state = systems[i]->getState();
        for (size_t k = 0; k < state.size(); ++k) {
            const glm::vec3& v = state[k];

            // NaN never equals itself; this also catches infinities below.
            if (v.x != v.x || v.y != v.y || v.z != v.z) return true;
            if (std::fabs(v.x) > limit ||
                std::fabs(v.y) > limit ||
                std::fabs(v.z) > limit) return true;
        }
    }
    return false;
}

} // namespace

void AnimationController::update(ShapeManager& shapeManager, TimeStepper* stepper) {
    if (!stepper) return;

    const bool playing = stepper->isAnimationPlaying();
    if (!playing && !stepOnce) return;

    std::vector<ParticleSystem*> systems = shapeManager.getParticleSystems();
    if (systems.empty()) {
        stepOnce = false;
        return;
    }

    // A single-step request always advances exactly one step, whatever the
    // substep or speed settings say - that is the point of it.
    int steps = 1;
    if (playing && !stepOnce) {
        float scaled = static_cast<float>(substeps) * (speed > 0.0f ? speed : 0.0f);
        steps = static_cast<int>(scaled + 0.5f);
        if (steps < 0) steps = 0;
        if (steps > 1000) steps = 1000;   // guard against a runaway UI value
    }

    for (int s = 0; s < steps; ++s) {
        for (size_t i = 0; i < systems.size(); ++i) {
            stepper->takeStep(systems[i], stepSize);
        }
        simTime += stepSize;
        ++stepCount;

        if (guardEnabled && hasDiverged(systems, guardLimit)) {
            stepper->stopAnimation();
            diverged = true;

            std::ostringstream msg;
            msg << "Simulation diverged at t = " << simTime
                << " s (step " << stepCount << "). "
                << "Reduce the step size or use a more stable integrator.";
            status = msg.str();
            break;
        }
    }

    stepOnce = false;
}

void AnimationController::reset(ShapeManager& shapeManager, TimeStepper* stepper) {
    if (stepper) stepper->stopAnimation();

    std::vector<ParticleSystem*> systems = shapeManager.getParticleSystems();
    for (size_t i = 0; i < systems.size(); ++i) {
        if (systems[i]) systems[i]->reset();
    }

    simTime   = 0.0;
    stepCount = 0;
    diverged  = false;
    stepOnce  = false;
    status.clear();
}
