#include "ParticleSystem.h"
#include "TimeStepper.h"
#include "SimpleSystem.h"

// Constructor initializes the animation state
TimeStepper::TimeStepper() : animationPlaying(false), stepSize(0.02f)  {}

// Play animation
void TimeStepper::playAnimation() {
    animationPlaying = true;
}

// Stop animation
void TimeStepper::stopAnimation() {
    animationPlaying = false;
}

// Reset animation (default behavior)
void TimeStepper::resetAnimation() {
    // Derived classes can override this to reset specific states
    stopAnimation();
	
}

// Check if the animation is playing
bool TimeStepper::isAnimationPlaying() const {
    return animationPlaying;
}

// Getters and setters for step size
float TimeStepper::getStepSize() const {
    return stepSize;
}

void TimeStepper::setStepSize(float newStepSize) {
    stepSize = newStepSize;
}


// Factory Method for Creating Integrators
TimeStepper* TimeStepper::createIntegrator(IntegratorType type) {
    switch (type) {
        case IntegratorType::ForwardEuler:
            return new ForwardEuler();
        case IntegratorType::Trapezoidal:
            return new Trapezoidal();
        case IntegratorType::Midpoint:
            return new Midpoint();
        case IntegratorType::RK4:
            return new RK4();
        default:
            return nullptr;
    }
}
