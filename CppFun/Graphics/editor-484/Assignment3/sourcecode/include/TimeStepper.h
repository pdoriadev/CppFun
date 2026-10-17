#ifndef TIMESTEPPER_H
#define TIMESTEPPER_H

#include "ParticleSystem.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>


enum class IntegratorType {
    ForwardEuler,
    Midpoint,
    Trapezoidal,
    RK4
};

class TimeStepper {
public:
    TimeStepper();
    virtual ~TimeStepper() = default;

    // Perform one step of simulation
    virtual void takeStep(ParticleSystem* particleSystem, float stepSize) = 0;

    // Animation controls
    void playAnimation();
    void stopAnimation();
    void resetAnimation();

    // Animation state
    bool isAnimationPlaying() const;

    // Getter and setter for step size
    float getStepSize() const;
    void setStepSize(float newStepSize);

	// Integrator factory
    static TimeStepper* createIntegrator(IntegratorType type);

private:
    bool animationPlaying;  // Controls whether the animation is active
    float stepSize;
};

// Forward Euler Integrator
class ForwardEuler : public TimeStepper {
public:
    void takeStep(ParticleSystem* particleSystem, float stepSize) override;
};

// Trapezoidal Integrator
class Trapezoidal : public TimeStepper {
public:
    void takeStep(ParticleSystem* particleSystem, float stepSize) override;
};

// Midpoint Integrator
class Midpoint : public TimeStepper {
public:
    void takeStep(ParticleSystem* particleSystem, float stepSize) override;
};

// Provided RK4 Integrator
class RK4 : public TimeStepper {
public:
    void takeStep(ParticleSystem* particleSystem, float stepSize) override;
};

#endif
