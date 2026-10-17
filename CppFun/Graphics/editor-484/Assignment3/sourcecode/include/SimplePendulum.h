#ifndef SIMPLEPENDULUM_H
#define SIMPLEPENDULUM_H

#include "PendulumSystem.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class SimplePendulum : public PendulumSystem {
public:
    const char* serialType() const override { return "SimplePendulum"; }
    // Constructor
    SimplePendulum(float x, float y, float z, float scale, int colorIndex, int id, float length, float mass);
    ~SimplePendulum();

private:
    float m_length;
    float m_mass;
};

#endif // SIMPLEPENDULUM_H
