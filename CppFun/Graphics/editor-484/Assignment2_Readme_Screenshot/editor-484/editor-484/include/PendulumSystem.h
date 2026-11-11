#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include "ParticleSystem.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class PendulumSystem : public ParticleSystem {
public:
    float getLength() const { return m_length; }
    // Constructor
    PendulumSystem(float x, float y, float z, float scale, int colorIndex, int id, int numParticles);
   ~PendulumSystem();
   
    // Evaluate forces and return derivatives (for animation)
    std::vector<glm::vec3> evalF(const std::vector<glm::vec3>& state) override; // Compute derivatives

    // Override draw method to render pendulum particles and strings
    void draw(GLuint shaderProgram) override;        

    // Reset to initial state
    void reset() override;              

    // Update particles after every step
    void updateParticles();

    // Convert deg to rad
    // double convertRad(int d) { return d*(M_PI/180.0); };

    // Enable and disable wind
    void enableWind();
    void disableWind();
    bool getWind() const;

    // Get and set wind intensity and direction
    void setWindDirection(const glm::vec3& direction); // Set wind direction
    void setWindIntensity(float intensity);            // Set wind intensity
    glm::vec3 getWindDirection() const;                // Get wind direction
    float getWindIntensity() const;                    // Get wind intensity

    // Enable and disable movement
    void enableMovement();   
    void disableMovement();   
    bool getMovement() const; 

    // Enable and disable wireframe
    void enableWireframe();    
    void disableWireframe();   
    bool getWireframe() const; 

    // Particles visibility
    void enableParticles();
    void disableParticles();
    bool getParticles() const;

    // Structural springs visibility
    void enableStructSprings();
    void disableStructSprings();
    bool getStructSprings() const;

    // Getter and Setter for mass
    void setMass(float mass);  // Set the mass
    float getMass() const;     // Get the mass
    
    

protected:
    float m_length;     // Length of the pendulum
    float m_mass;       // Mass of the pendulum
    float m_gravity;    // Gravity Constant
    float m_drag;       // Drag of the pendulum
    glm::vec3 m_anchor; // Fixed anchor point

    bool wireframe_ON;
    bool wind_ON;
    bool sinusoidMove_ON;
    bool faces_ON;
    bool particles_ON;
    bool structSprings_ON;
    bool randomWind_ON;
    
    int degree;

    glm::vec3 windDirection; // Wind direction (normalized)
    float windIntensity;     // Wind intensity

    bool isCloth;

    //Particle ( vec3 pos, bool fixed)
    std::vector<glm::vec4> particles;

    //Spring ( from p0, to p1, restLength, springConstant) 
    std::vector<glm::vec4> springs;

    //Particle ( vec3 pos)
    std::vector<glm::vec3> faces;
    
    
    // Generate particle template
    void generateUnitSphereMesh(float radius, int sectorCount, int stackCount);    

    // Particle sphere rendering
    GLuint VAO, VBO, EBO;
    std::vector<float> particleVertices;
    std::vector<unsigned int> particleIndices;

    // Line rendering
    GLuint springVAO, springVBO;
    std::vector<float> springVertices;

    // Wireframe rendering
    GLuint wireVAO, wireVBO;
    std::vector<float> wireVertices;

    // Faces rendering
    GLuint faceVAO, faceVBO, faceEBO;
    std::vector<float> faceVertices;
    std::vector<unsigned int> faceIndices;

    void setupParticles();
    void setupSprings();
    void setupWireframe();
    void setupFaces();   
    
    void updateSprings();
    void updateWireframe();
    void updateFaces(); 

    // Unit sphere base mesh
    std::vector<glm::vec3> unitSphereVertices;
    std::vector<glm::vec3> unitSphereNormals;
    std::vector<unsigned int> unitSphereIndices;    
    
    //Construct Specific Pendulum
    void setupParticles(const std::vector<glm::vec4>& myParticles, 
                        const std::vector<glm::vec4>& mySprings, 
                        const std::vector<glm::vec3>& myFaces);
    

};

#endif // SIMPLEPENDULUM_H
