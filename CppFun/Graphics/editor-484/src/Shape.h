#ifndef SHAPE_H
#define SHAPE_H
#endif
#include <cmath>
#include <glm/glm.hpp>


enum class ShapeType {
    ShapeType = -100,
    INVALID = -1,
    TRI = 0,
    SQUARE,    
    CIRCLE,
    SPHERE,
    CUBE,
    ICOSAHEDRON,
    COUNT
};


//-///////////////////////////////////////////////
class Shape {
public:
    // Default construction values. 
    ShapeType type = ShapeType::INVALID;
    glm::mat4 modelMatrix = glm::mat4(1.0);
    
    Shape(ShapeType);
    
    glm::mat4 getModelMatrix() const;
};