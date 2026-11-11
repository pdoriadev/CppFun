// #ifndef SHAPE_H
// #define SHAPE_H
// #endif
// #include <glm/glm.hpp>


// enum class ShapeType {
//     ShapeType = -100,
//     INVALID = -1,
//     TRI = 0,
//     SQUARE,    
//     CIRCLE,
//     SPHERE,
//     CUBE,
//     ICOSAHEDRON,
//     COUNT
// };


// //-///////////////////////////////////////////////
// //
// class Shape {
// public:
//     // Default construction values. 
//     ShapeType type = ShapeType::INVALID;
//     glm::mat4 m4_pos = glm::mat4(1.0f);
//     glm::mat4 m4_rot = glm::mat4(1.0f);
//     glm::mat4 m4_scale = glm::mat4(1.0f);
    
//     Shape(ShapeType, glm::mat4 _modelMatrix = glm::mat4(1.0f));
    
//     glm::mat4 getModelMatrix() const;
// };