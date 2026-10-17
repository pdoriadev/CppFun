#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(float x, float y, float z, float scale, int colorIndex, int id)
    : Shape(x, y, z, scale, colorIndex, id), m_numParticles(0) {}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::addParticle(const glm::vec3& position, const glm::vec3& velocity) {
    m_state.push_back(position);
    m_state.push_back(velocity);
    m_initialState.push_back(position);
    m_initialState.push_back(velocity);
    m_numParticles++;
}

void ParticleSystem::reset() {
    // Reset state to the initial state
    m_state = m_initialState;
}

std::vector<glm::vec3> ParticleSystem::getState() const {
    return m_state;
}

void ParticleSystem::setState(const std::vector<glm::vec3>& newState) {
    m_state = newState;
}
