#ifndef SIMPLESYSTEM_H
#define SIMPLESYSTEM_H

#include "ParticleSystem.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cmath> // For math functions like sin, cos, sqrt


class SimpleSystem : public ParticleSystem {
public:
    const char* serialType() const override { return "SimpleSystem"; }
    // Constructor
    SimpleSystem(float x, float y, float z, float scale, int colorIndex, int id);
    ~SimpleSystem();
    
    // Evaluate forces and return derivatives (for animation)
    std::vector<glm::vec3> evalF(const std::vector<glm::vec3>& state) override;

    // Override draw method to render pendulum particles and strings
    void draw(GLuint shaderProgram) override;        

    // Reset to initial state
    void reset() override;              

    // Update particles after every step
    void updateParticles();

private:

    // Generate particle template
    void generateUnitSphereMesh(float radius, int sectorCount, int stackCount);

    // Particle sphere rendering
    GLuint VAO, VBO, EBO;
    std::vector<float> particleVertices;
    std::vector<unsigned int> particleIndices;

    void setupParticles();
    
    // Unit sphere base mesh
    std::vector<glm::vec3> unitSphereVertices;
    std::vector<glm::vec3> unitSphereNormals;
    std::vector<unsigned int> unitSphereIndices;    
    
};

#endif // SIMPLESYSTEM_H

