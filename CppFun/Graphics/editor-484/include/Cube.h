#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class PlaneType : int32_t {
    PLANE_TYPE = -100,
    INVALID = -1,
    TOP = 0,
    INTERMEDIATE,
    BOTTOM,
    COUNT
};

std::string getPlaneTypeString(PlaneType type);

struct PlaneConstructionParams {
    PlaneType const type;
    uint32_t const MAX_LOOPS;
    uint32_t const VERTS_PER_LOOP;
    uint32_t const VALUES_PER_VERT;
    uint32_t const LOOPS_COMPLETED_I;
    uint32_t const LOOP_PROGRESS_J;
    std::vector<unsigned int>& elementData; // index buffer
    std::vector<float>& vertexData; // raw vertex data

    PlaneConstructionParams(
        PlaneType const _type, 
        uint32_t const _MAX_LOOPS,
        uint32_t const _VERTS_PER_LOOP,
        uint32_t const _VALUES_PER_VERT, 
        uint32_t const _LOOPS_COMPLETED_I,
        uint32_t const _LOOP_PROGRESS_J,
        std::vector<unsigned int>& _elementData,
        std::vector<float>& _vertexData) 
        :
        type(_type),
        MAX_LOOPS(_MAX_LOOPS),
        VERTS_PER_LOOP(_VERTS_PER_LOOP),
        VALUES_PER_VERT(_VALUES_PER_VERT),
        LOOPS_COMPLETED_I(_LOOPS_COMPLETED_I),
        LOOP_PROGRESS_J(_LOOP_PROGRESS_J),
        elementData(_elementData), 
        vertexData(_vertexData){}
};

class Cube : public Shape {
public:
    const char* serialType() const override { return "Cube"; }
    Cube(float x, float y, float z, float scale, int colorIndex, int id);
    ~Cube();

    void draw(GLuint shaderProgram) override; // Render the cube
    
    private:
    GLuint VAO, VBO, EBO; // OpenGL handles for the cube geometry
    bool setupCube();     // Initializes the VAO/VBO/EBO for the cube
    
    bool constructPlane(PlaneConstructionParams);

    
};

#endif
