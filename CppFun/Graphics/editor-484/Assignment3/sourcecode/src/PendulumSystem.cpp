#include "PendulumSystem.h"
#include <cmath> // For math functions like sin, cos, sqrt
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>


PendulumSystem::PendulumSystem(float x, float y, float z, float scale, int colorIndex, int id, int numParticles)
    : ParticleSystem(x, y, z, scale, colorIndex, id), windDirection(1.0f, 0.0f, 0.0f), windIntensity(1.0f) { 

    m_numParticles = numParticles;
    m_gravity = -9.81f;
    m_drag = 1.0f;
    m_mass = 1.0f;

    wireframe_ON = false;
    wind_ON = false;
    sinusoidMove_ON = false;
    faces_ON = true;

    particles_ON = false;
    structSprings_ON = false;

    isCloth = false;

    degree = 0;

    float radius = 0.025f;
    int sectorCount = 8;
    int stackCount = 8;

    // Particle sphere rendering.
    //
    // These were previously written as `VAO, VBO, EBO = 0;`. That is the comma
    // operator, not a multiple assignment: it evaluates VAO, discards it,
    // evaluates VBO, discards it, and assigns 0 to EBO alone. Seven of the ten
    // handles below were left holding whatever was on the heap.
    //
    // It is reachable, not theoretical. setupWireframe() returns early when the
    // particle count is not a perfect square (it prints "m_numParticles must be
    // a perfect square"), so wireVAO/wireVBO never reach their glGen calls, and
    // ~PendulumSystem then runs `if (wireVAO) glDeleteVertexArrays(1, &wireVAO)`
    // on garbage. If that garbage happens to collide with a live VAO belonging
    // to another shape, this deletes someone else's buffers.
    VAO = VBO = EBO = 0;
    springVAO = springVBO = 0;
    wireVAO = wireVBO = 0;
    faceVAO = faceVBO = faceEBO = 0;

    generateUnitSphereMesh( radius, sectorCount, stackCount);
}

PendulumSystem::~PendulumSystem() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (springVAO) glDeleteVertexArrays(1, &springVAO);
    if (springVBO) glDeleteBuffers(1, &springVBO);
    if (wireVAO) glDeleteVertexArrays(1, &wireVAO);
    if (wireVBO) glDeleteBuffers(1, &wireVBO);
    if (faceVAO) glDeleteVertexArrays(1, &faceVAO);
    if (faceVBO) glDeleteBuffers(1, &faceVBO);
    if (faceEBO) glDeleteBuffers(1, &faceEBO);
}


void PendulumSystem::generateUnitSphereMesh(float radius, int sectorCount, int stackCount) {
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


void PendulumSystem::setupParticles(const std::vector<glm::vec4>& myParticles, 
                                    const std::vector<glm::vec4>& mySprings,
                                    const std::vector<glm::vec3>& myFaces) {

    particleVertices.clear();
    particleIndices.clear();
    springVertices.clear();
    
    
    particles = myParticles;
    springs = mySprings;
    faces = myFaces;

    m_state.clear();

    // Initialize particles and velocities in m_state
    for (size_t i = 0; i < particles.size(); i++) {
        m_state.push_back(glm::vec3(particles[i]));  // Particle position
        m_state.push_back(glm::vec3(0.0f, 0.0f, 0.0f)); // Initial velocity
    }

    //Save initial state for reset
    m_initialState = m_state;


    // Build particle indices and vertices
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
    
    
    // Build the buffers for the particles

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
    
    setupSprings();
    setupFaces();
    setupWireframe();

}

void PendulumSystem::setupSprings() {

    springVertices.clear();

    for (const auto& spring : springs) {
        int i0 = static_cast<int>(spring[0]);
        int i1 = static_cast<int>(spring[1]);

        glm::vec3 p0 = m_state[2 * i0]; // particle positions
        glm::vec3 p1 = m_state[2 * i1];

        springVertices.insert(springVertices.end(), { p0.x, p0.y, p0.z });
        springVertices.insert(springVertices.end(), { p1.x, p1.y, p1.z });
    }
    
    glGenVertexArrays(1, &springVAO);
    glGenBuffers(1, &springVBO);

    glBindVertexArray(springVAO);
    glBindBuffer(GL_ARRAY_BUFFER, springVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * springs.size(), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    updateSprings();

}


void PendulumSystem::setupWireframe() {
    wireVertices.clear();

    int gridWidth = static_cast<int>(std::sqrt(m_numParticles));
    if (gridWidth * gridWidth != m_numParticles) {
        std::cerr << "setupWireframe: m_numParticles must be a perfect square for grid layout." << std::endl;
        return;
    }


    for (int row=0; row < gridWidth; row++){
        for (int col=0; col < gridWidth; col++){

	    if (row < gridWidth-1){
	        glm::vec3 p0 = particles[(float)((row)   * gridWidth + (col))];
	        glm::vec3 p1 = particles[(float)((row+1) * gridWidth + (col))];
	        
                      wireVertices.insert(wireVertices.end(), { p0.x, p0.y, p0.z });
                      wireVertices.insert(wireVertices.end(), { p1.x, p1.y, p1.z });		    
	    }

	    if (col < gridWidth-1){
	        glm::vec3 p0 = particles[(float)((row) * gridWidth + (col)  )];
	        glm::vec3 p1 = particles[(float)((row) * gridWidth + (col+1))];
	        
                      wireVertices.insert(wireVertices.end(), { p0.x, p0.y, p0.z });
                      wireVertices.insert(wireVertices.end(), { p1.x, p1.y, p1.z });		    
	    }

        }

    }

    // Generate VAO and VBO if not already created
    glGenVertexArrays(1, &wireVAO);
    glGenBuffers(1, &wireVBO);

    // Bind VAO and VBO
    glBindVertexArray(wireVAO);
    glBindBuffer(GL_ARRAY_BUFFER, wireVBO);

    // Upload wireframe vertex data
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * wireVertices.size(), nullptr, GL_DYNAMIC_DRAW);

    
    // Position attribute (3 floats per vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind VAO
    
    updateWireframe();    
   
}




void PendulumSystem::setupFaces() {

    faceVertices.clear();
    faceIndices.clear();

    for (int i = 0; i < m_numParticles; ++i) {
        glm::vec3 pos = m_state[2 * i]; 
        glm::vec3 normal(0.0f, 1.0f, 0.0f); // you can compute actual normals later

        faceVertices.insert(faceVertices.end(), {
            pos.x, pos.y, pos.z,
            normal.x, normal.y, normal.z
        });
    }

    for (const auto& face : faces) {
        faceIndices.push_back(static_cast<unsigned int>(face.x));
        faceIndices.push_back(static_cast<unsigned int>(face.y));
        faceIndices.push_back(static_cast<unsigned int>(face.z));
    }

    // Build the buffers for the particles

    glGenVertexArrays(1, &faceVAO);
    glGenBuffers(1, &faceVBO);
    glGenBuffers(1, &faceEBO);

    glBindVertexArray(faceVAO);

    glBindBuffer(GL_ARRAY_BUFFER, faceVBO);
    glBufferData(GL_ARRAY_BUFFER, faceVertices.size() * sizeof(float), faceVertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceIndices.size() * sizeof(unsigned int), faceIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    updateFaces();    

}


void PendulumSystem::updateParticles() {
    particleVertices.clear(); // Reset

    for (int p = 0; p < m_numParticles; ++p) {
        glm::vec3 center = m_state[2 * p]; // current particle position

        for (size_t i = 0; i < unitSphereVertices.size(); ++i) {
            glm::vec3 pos = unitSphereVertices[i] + center;
            glm::vec3 normal = unitSphereNormals[i];

            // Insert 6 floats per vertex
            particleVertices.insert(particleVertices.end(), {
                pos.x, pos.y, pos.z,
                normal.x, normal.y, normal.z
            });
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particleVertices.size(), particleVertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}


void PendulumSystem::updateSprings() {
    springVertices.clear();

    for (const auto& spring : springs) {
        int i0 = static_cast<int>(spring[0]);
        int i1 = static_cast<int>(spring[1]);

        glm::vec3 p0 = m_state[2 * i0]; // particle positions
        glm::vec3 p1 = m_state[2 * i1];

        springVertices.insert(springVertices.end(), { p0.x, p0.y, p0.z });
        springVertices.insert(springVertices.end(), { p1.x, p1.y, p1.z });
    }

    glBindBuffer(GL_ARRAY_BUFFER, springVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * springVertices.size(), springVertices.data(), GL_DYNAMIC_DRAW); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PendulumSystem::updateWireframe() {
    wireVertices.clear();

    int gridWidth = static_cast<int>(std::sqrt(m_numParticles));
    if (gridWidth * gridWidth != m_numParticles) {
        std::cerr << "updateWireframe: m_numParticles must be a perfect square for grid layout." << std::endl;
        return;
    }

    for (int row = 0; row < gridWidth; ++row) {
        for (int col = 0; col < gridWidth; ++col) {
            int index = row * gridWidth + col;

            if (row < gridWidth - 1) {
                glm::vec3 p0 = m_state[2 * index];                           // Use m_state
                glm::vec3 p1 = m_state[2 * ((row + 1) * gridWidth + col)];
                wireVertices.insert(wireVertices.end(), { p0.x, p0.y, p0.z });
                wireVertices.insert(wireVertices.end(), { p1.x, p1.y, p1.z });
            }

            if (col < gridWidth - 1) {
                glm::vec3 p0 = m_state[2 * index];
                glm::vec3 p1 = m_state[2 * (row * gridWidth + (col + 1))];
                wireVertices.insert(wireVertices.end(), { p0.x, p0.y, p0.z });
                wireVertices.insert(wireVertices.end(), { p1.x, p1.y, p1.z });
            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, wireVBO);
    glBufferData(GL_ARRAY_BUFFER, wireVertices.size() * sizeof(float), wireVertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}





void PendulumSystem::updateFaces() {
    faceVertices.clear();
    faceIndices.clear();

    // 1. Initialize per-particle normals
    std::vector<glm::vec3> vertexNormals(m_numParticles, glm::vec3(0.0f));

    // 2. Loop over each face and accumulate area-weighted normals
    for (const auto& face : faces) {
        int i0 = static_cast<int>(face.x);
        int i1 = static_cast<int>(face.y);
        int i2 = static_cast<int>(face.z);

        glm::vec3 v0 = m_state[2 * i0];
        glm::vec3 v1 = m_state[2 * i1];
        glm::vec3 v2 = m_state[2 * i2];

        glm::vec3 faceNormal = glm::cross(v1 - v0, v2 - v0);
        float area = glm::length(faceNormal) * 0.5f;

        if (area > 0.0f) {
            glm::vec3 weightedNormal = glm::normalize(faceNormal) * area;

            vertexNormals[i0] += weightedNormal;
            vertexNormals[i1] += weightedNormal;
            vertexNormals[i2] += weightedNormal;
        }

        faceIndices.push_back(i0);
        faceIndices.push_back(i1);
        faceIndices.push_back(i2);
    }

    // 3. Normalize the accumulated normals and build faceVertices
    for (int i = 0; i < m_numParticles; ++i) {
        glm::vec3 pos = m_state[2 * i];
        glm::vec3 normal = glm::normalize(vertexNormals[i]);

        faceVertices.insert(faceVertices.end(), {
            pos.x, pos.y, pos.z,
            normal.x, normal.y, normal.z
        });
    }

    // 4. Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, faceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * faceVertices.size(), faceVertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}




// TODO: implement evalF
// for a given state, evaluate f(X,t)





// Enable wind
void PendulumSystem::enableWind() { wind_ON = true; }

// Disable wind
void PendulumSystem::disableWind() { wind_ON = false; }

// Get wind status
bool PendulumSystem::getWind() const{ return wind_ON; }

// Set wind direction
void PendulumSystem::setWindDirection(const glm::vec3& direction) {
    windDirection = glm::normalize(direction);
}

// Set wind intensity
void PendulumSystem::setWindIntensity(float intensity) {
    windIntensity = intensity;
}

// Get wind direction
glm::vec3 PendulumSystem::getWindDirection() const {
    return windDirection;
}

// Get wind intensity
float PendulumSystem::getWindIntensity() const {
    return windIntensity;
}

// Movement utilities
void PendulumSystem::enableMovement() { sinusoidMove_ON = true; }
void PendulumSystem::disableMovement() { sinusoidMove_ON = false; }
bool PendulumSystem::getMovement() const { return sinusoidMove_ON; }

// Wireframe utilities
void PendulumSystem::enableWireframe() { wireframe_ON = true; faces_ON = false; }
void PendulumSystem::disableWireframe() { wireframe_ON = false; faces_ON = true; }
bool PendulumSystem::getWireframe() const { return wireframe_ON; }

// Particles utilities
void PendulumSystem::enableParticles() { particles_ON = true; }
void PendulumSystem::disableParticles() { particles_ON = false; }
bool PendulumSystem::getParticles() const { return particles_ON; }

// Structural springs utilities
void PendulumSystem::enableStructSprings() { structSprings_ON = true; }
void PendulumSystem::disableStructSprings() { structSprings_ON = false; }
bool PendulumSystem::getStructSprings() const { return structSprings_ON; }

void PendulumSystem::setMass(float mass) { m_mass = mass; }
float PendulumSystem::getMass() const { return m_mass; }


void PendulumSystem::reset() {
    // Reset the state of the pendulum using the parent class's reset method
    ParticleSystem::reset();
    
    m_state = m_initialState;  // Restore positions and velocities
    updateParticles();         // Update buffers
    updateSprings();
    updateWireframe();
    updateFaces();
}

void PendulumSystem::draw(GLuint shaderProgram) {

    updateSprings();
    updateWireframe();
    updateFaces(); // optional

    // Use the shader program
    glUseProgram(shaderProgram);
    
    // Enable lighting for the cube
    GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 1); // Enable lighting for the pendulum
    }
    
    applyTransform(shaderProgram);

    // Set material color in shader
    GLint colorLoc = glGetUniformLocation(shaderProgram, "material.color");
    if (colorLoc != -1) {
        glUniform3fv(colorLoc, 1, (colorIndex == 31) ? customColor : colorPresets[colorIndex].color);
    }

    // Draw Faces (as triangles)
    
    if (faces_ON) {
    
        // Set material color in shader
        GLint colorLoc = glGetUniformLocation(shaderProgram, "material.color");
        if (colorLoc != -1) {
            glUniform3fv(colorLoc, 1, (colorIndex == 31) ? customColor : colorPresets[colorIndex].color);
        }
        
        glBindVertexArray(faceVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(faceIndices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
            
    }
    

    // Wireframe

    if (wireframe_ON){

        // Draw Particles if enabled
        if (particles_ON) {
        
            GLint colorLoc = glGetUniformLocation(shaderProgram, "material.color");
            if (colorLoc != -1) {
                glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // white lines
            }
            
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(particleIndices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        
            // Draw Structural Springs (as lines)
        if (structSprings_ON) {
        
            // Draw springs as flat, unlit lines - like the axis lines.
            //
            // This used to pass 1 here, which ENABLES lighting despite the
            // comment. The spring VAO supplies only positions (no normals), so
            // the shader normalized a zero normal, diffuse and specular both
            // collapsed, and the springs came out at ambient brightness only -
            // barely visible against the background.
            GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
            if (lightingLoc != -1) {
                glUniform1i(lightingLoc, 0);
            }

            glBindVertexArray(springVAO);
            // The unlit path reads FragColor, which comes from attribute 2.
            // That array is disabled for this VAO, so the shader falls back to
            // the constant generic value set here - no extra VBO needed.
            glVertexAttrib3f(2, 0.25f, 1.0f, 0.55f);   // green: structural springs
            glLineWidth(2.0f);
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(springVertices.size() / 3));
            glBindVertexArray(0);

            if (lightingLoc != -1) glUniform1i(lightingLoc, 1);

        } else {
        
            // Same fix as the structural springs above: genuinely unlit, with
            // an explicit constant colour so the mesh reads clearly.
            GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
            if (lightingLoc != -1) {
                glUniform1i(lightingLoc, 0);
            }

            glBindVertexArray(wireVAO);
            glVertexAttrib3f(2, 0.45f, 0.8f, 1.0f);    // blue: structural wireframe
            glLineWidth(2.0f);
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(wireVertices.size() / 3));
            glBindVertexArray(0);

            if (lightingLoc != -1) glUniform1i(lightingLoc, 1);
        
        }


    }
    
}

