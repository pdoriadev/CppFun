#ifndef SURFACE_H
#define SURFACE_H

#include "Curve.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

// Define Tup3u struct for storing triangle indices
struct Tup3u {
    unsigned v[3];
    Tup3u(unsigned a, unsigned b, unsigned c) { v[0] = a; v[1] = b; v[2] = c; }
};


// Class to represent a surface generated from curves
class Surface {
public:
    // Static methods to generate surfaces
    static Surface makeSurfRev(const Curve& profile, unsigned steps);
    static Surface makeGenCyl(const Curve& profile, const Curve& sweep);

    void draw();

    std::vector<glm::vec3> VV;  // Vertices
    std::vector<glm::vec3> VN;  // Normals
    std::vector<Tup3u> VF;      // Faces (triangle indices)

private:
    GLuint VAO, VBO, NBO, EBO;
    void setupBuffers();
    
};

#endif  // SURFACE_H
