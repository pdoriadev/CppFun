
#include "SimpleSystem.h"
#include <iostream>

SimpleSystem::SimpleSystem(float x, float y, float z, float scale, int colorIndex, int id)
    : ParticleSystem(x, y, z, scale, colorIndex, id) {

    shapeType = "Simple System";

    // Initialize particle state
    m_state.push_back(glm::vec3(0.0f, 0.0f, 0.0f)); // Position
    m_state.push_back(glm::vec3(0.0f, 0.0f, 0.0f)); // Velocity
    m_state.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // Position
    m_state.push_back(glm::vec3(0.0f, 0.0f, 0.0f)); // Velocity

    m_initialState = m_state;
    m_numParticles = 2;

    float radius = 0.025f;
    int sectorCount = 8;
    int stackCount = 8;

    generateUnitSphereMesh( radius, sectorCount, stackCount);
    setupParticles();
}

SimpleSystem::~SimpleSystem() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}


void SimpleSystem::generateUnitSphereMesh(float radius, int sectorCount, int stackCount) {
    unitSphereVertices.clear();
    unitSphereNormals.clear();
    unitSphereIndices.clear();

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stackCount;
        float xy = radius * cos(stackAngle);
        float z = radius * sin(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * glm::pi<float>() / sectorCount;
            float x = xy * cos(sectorAngle);
            float y = xy * sin(sectorAngle);
            glm::vec3 pos(x, y, z);
            glm::vec3 normal = glm::normalize(pos);
            unitSphereVertices.push_back(pos);
            unitSphereNormals.push_back(normal);
        }
    }

    for (int i = 0; i < stackCount; ++i) {
        for (int j = 0; j < sectorCount; ++j) {
            int first = i * (sectorCount + 1) + j;
            int second = first + sectorCount + 1;

            unitSphereIndices.push_back(first);
            unitSphereIndices.push_back(second);
            unitSphereIndices.push_back(first + 1);

            unitSphereIndices.push_back(second);
            unitSphereIndices.push_back(second + 1);
            unitSphereIndices.push_back(first + 1);
        }
    }
}



void SimpleSystem::setupParticles() {
    particleVertices.clear();
    particleIndices.clear();

    int vertexOffset = 0;
    int sphereVertexCount = static_cast<int>(unitSphereVertices.size());

    for (int p = 0; p < m_numParticles; ++p) {
    
        glm::vec3 center = m_state[2 * p]; // position (even index)        

        for (size_t i = 0; i < unitSphereVertices.size(); ++i) {
            glm::vec3 pos = unitSphereVertices[i] + center;
            glm::vec3 normal = unitSphereNormals[i];

            particleVertices.insert(particleVertices.end(), { pos.x, pos.y, pos.z });
            particleVertices.insert(particleVertices.end(), { normal.x, normal.y, normal.z });
        }

        for (size_t i = 0; i < unitSphereIndices.size(); ++i) {
            particleIndices.push_back(static_cast<unsigned int>(vertexOffset + unitSphereIndices[i]));
        }

        vertexOffset += sphereVertexCount;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, particleVertices.size() * sizeof(float), particleVertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, particleIndices.size() * sizeof(unsigned int), particleIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}


void SimpleSystem::updateParticles() {
    int vertexOffset = 0;
    int sphereVertexCount = static_cast<int>(unitSphereVertices.size());

    for (int p = 0; p < m_numParticles; ++p) {
        glm::vec3 center = m_state[2 * p]; // position (even index)

        for (size_t i = 0; i < unitSphereVertices.size(); ++i) {
            glm::vec3 pos = unitSphereVertices[i] + center;
            int baseIdx = (vertexOffset + i) * 6;
            particleVertices[baseIdx + 0] = pos.x;
            particleVertices[baseIdx + 1] = pos.y;
            particleVertices[baseIdx + 2] = pos.z;
        }

        vertexOffset += sphereVertexCount;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * particleVertices.size(), particleVertices.data());
}


std::vector<glm::vec3> SimpleSystem::evalF(const std::vector<glm::vec3>& state) {
    std::vector<glm::vec3> f;

    for (int i = 0; i < m_numParticles; ++i) {
        glm::vec3 pos = state[2 * i];         // actual particle state
        glm::vec3 dummy = state[2 * i + 1];   // unused, just for consistency

        glm::vec3 dPos = glm::vec3(-pos.y, pos.x, 0.0f);  // simulating circular field
        glm::vec3 dDummy(0.0f);  // no change in velocity since it's unused

        f.push_back(dPos);   // dx/dt = velocity-like value
        f.push_back(dDummy); // dv/dt = dummy (zero)
    }

    return f;
}

void SimpleSystem::reset() {
    ParticleSystem::reset();
}

void SimpleSystem::draw(GLuint shaderProgram) {

    // Use the shader program
    glUseProgram(shaderProgram);
    
    // Enable lighting for the cube
    GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 1); // Enable lighting for the pendulum
    }
    
    applyTransform(shaderProgram);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "material.color");

    if (colorLoc != -1) {
        glUniform3fv(colorLoc, 1, (colorIndex == 31) ? customColor : colorPresets[colorIndex].color);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(particleIndices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


