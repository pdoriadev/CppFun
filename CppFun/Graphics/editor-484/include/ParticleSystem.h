#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "Shape.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class ParticleSystem : public Shape {
public:
    ParticleSystem(float x, float y, float z, float scale, int colorIndex, int id);
    virtual ~ParticleSystem();

    // Virtual function for computing particle system derivatives
    virtual std::vector<glm::vec3> evalF(const std::vector<glm::vec3>& state) = 0;

    // Update particle state after intergrator step
    virtual void updateParticles() {};

    // Reset the particle system to its initial state
    virtual void reset();

    // Getters and setters for particle states
    std::vector<glm::vec3> getState() const;
    void setState(const std::vector<glm::vec3>& newState);
    

protected:
    std::vector<glm::vec3> m_state;         // Particle positions and velocities
    std::vector<glm::vec3> m_initialState;  // Initial state for reset
    int m_numParticles;                     // Number of particles

    // Add particle to list of particles
    void addParticle(const glm::vec3& position, const glm::vec3& velocity);
};

#endif // PARTICLESYSTEM_H
