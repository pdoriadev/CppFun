#include "SimpleChain.h"
#include <vector>

SimpleChain::SimpleChain(float x, float y, float z, float scale, int colorIndex, int id, float length, float mass)
    : PendulumSystem(x, y, z, scale, colorIndex, id, 4)  // 2 particles: anchor and bob
{
	m_length = length;
	m_mass = mass;

	shapeType = "Simple Chain";  // Set the type as "Simple Chain"

    wireframe_ON = true;
    structSprings_ON = true;
    particles_ON = true;


    particles.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    particles.push_back(glm::vec4(-1.0f, 0.0f, 0.5f, 0.0f));
    particles.push_back(glm::vec4(1.0f, -1.0f, 0.0f, 0.0f));
    particles.push_back(glm::vec4(-0.0f, 1.0f, -1.5f, 0.0f));

    //float restLength = 1.0f;
    //float springConstant = 2.0f;

    springs.push_back(glm::vec4(0.0f, 1.0f, 0.05f, 12.0f));
    springs.push_back(glm::vec4(1.0f, 2.0f, 0.05f, 12.0f));
    springs.push_back(glm::vec4(2.0f, 3.0f, 0.05f, 12.0f));

    setupParticles(particles, springs, faces);

}


