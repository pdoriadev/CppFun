// =============================================================================
// Integrators -- ForwardEuler, Trapezoidal, Midpoint, RK4
// =============================================================================
// Split out of TimeStepper.cpp deliberately. TimeStepper.cpp keeps the parts
// that are always given -- construction, play/stop/reset, the step size, and
// the createIntegrator factory. This file holds only the four takeStep()
// implementations, which are the student's work.
//
// The split is what makes a per-assignment solution library possible: the unit
// the linker can swap is a whole translation unit, so the swappable code has to
// live in a file of its own.
//
// See SOLUTION-LIBRARIES.md.

#include "ParticleSystem.h"
#include "TimeStepper.h"

// Forward Euler Method
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize) {
    // NOT PART OF THIS COURSE -- this is particles material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // Get the state, evaluate f, step every element, set the state back.
    // Finish with particleSystem->updateParticles().
    (void)particleSystem; (void)stepSize;
}

// Trapezoidal Method
void Trapezoidal::takeStep(ParticleSystem* particleSystem, float stepSize) {
    // NOT PART OF THIS COURSE -- this is particles material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // f0 = f(state); predict with Euler; f1 = f(prediction);
    // newState = state + (h/2) * (f0 + f1).
    (void)particleSystem; (void)stepSize;
}

// Midpoint Method
void Midpoint::takeStep(ParticleSystem* particleSystem, float stepSize) {
    // NOT PART OF THIS COURSE -- this is particles material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // f0 = f(state); midState = state + (h/2)*f0;
    // newState = state + h * f(midState).
    (void)particleSystem; (void)stepSize;
}


// RK4 Method
void RK4::takeStep(ParticleSystem* particleSystem, float stepSize) {
    // NOT PART OF THIS COURSE -- this is particles material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // f1..f4 as in the handout, then
    // newState = state + (h/6) * (f1 + 2*f2 + 2*f3 + f4).
    (void)particleSystem; (void)stepSize;
}
