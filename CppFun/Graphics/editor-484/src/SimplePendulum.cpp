#include "SimplePendulum.h"
#include <vector>

SimplePendulum::SimplePendulum(float x, float y, float z, float scale, int colorIndex, int id, float length, float mass)
    : PendulumSystem(x, y, z, scale, colorIndex, id, 2), m_length(length), m_mass(mass) {

    shapeType = "Simple Pendulum";  // Set the type as "Simple Pendulum"

    wireframe_ON = true;
    structSprings_ON = true;
    particles_ON = true;

    // Define pendulum particles
    particles.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    particles.push_back(glm::vec4(1.0f, 1.0f, 0.5f, 0.0f));

    m_numParticles = 2;
    
    //float restLength = 1.0f;
    //float springConstant = 2.0f;

    // Define spring connecting the anchor to the bob
    springs.push_back(glm::vec4(0.0f, 1.0f, 1.0f, 15.0f));


    setupParticles(particles, springs, faces);
    
}

SimplePendulum::~SimplePendulum() {

}



