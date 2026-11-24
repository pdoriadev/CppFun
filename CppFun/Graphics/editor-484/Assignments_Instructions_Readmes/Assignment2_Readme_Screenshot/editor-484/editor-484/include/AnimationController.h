#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

#include <string>
#include "TimeStepper.h"

class ShapeManager;

// Owns simulation advancement, so the main loop stays a one-liner and the UI
// has somewhere to read state from.
//
// This used to live inline in Application::run(), which meant there was nowhere
// to keep simulation time, step counts, or a single-step request - the loop
// either ran or it did not.
struct AnimationController {

    // --- settings ---
    int   integrator;     // IntegratorType, mirrored so the UI can show it
    float stepSize;       // seconds per integration step
    int   substeps;       // integration steps per rendered frame
    float speed;          // playback multiplier applied to substep count

    // Divergence guard. Forward Euler at a large step size will blow up; left
    // unchecked the positions become NaN and the scene silently disappears.
    // Catching it and pausing turns that into something observable.
    bool  guardEnabled;
    float guardLimit;     // |position| beyond this counts as diverged

    // --- state ---
    double simTime;       // seconds of simulated time since the last reset
    long   stepCount;
    bool   stepOnce;      // single-step request, honoured while paused
    bool   diverged;
    std::string status;

    AnimationController()
        : integrator(static_cast<int>(IntegratorType::ForwardEuler)),
          stepSize(0.01f),
          substeps(1),
          speed(1.0f),
          guardEnabled(true),
          guardLimit(1.0e4f),
          simTime(0.0),
          stepCount(0),
          stepOnce(false),
          diverged(false) {}

    // Advances the simulation one frame's worth. Call once per frame.
    void update(ShapeManager& shapeManager, TimeStepper* stepper);

    // Restores every particle system to its initial state and zeroes the clock.
    void reset(ShapeManager& shapeManager, TimeStepper* stepper);

    void requestSingleStep() { stepOnce = true; diverged = false; }
};

extern AnimationController g_animation;

#endif // ANIMATIONCONTROLLER_H
