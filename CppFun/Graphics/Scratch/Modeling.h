#ifndef MODELING_H
#define MODELING_H
#include <glm/ext/vector_float3.hpp>
#endif

#include <vector>
#include <cmath>

// Look into alternative math library for later: GLM. Used and recommended by professor. 
//      OR second choice https://ggt.sourceforge.net/
class vector3 {
public:
    float vec_xyz[3];
    static vector3 const vec_zero;

    vector3();
    vector3(float _x, float _y, float _z);
    
    static bool is_equal(vector3 a, vector3 b, float errorRange);
    static float square_magnitude(vector3 a);
    static bool add_first_to_second(vector3 a, vector3& outVec);
    static vector3 add(vector3 a, vector3 b);
    static vector3 scale_uniform(float s, vector3 vec);
    static bool modulo(int m, vector3& outVec);
    //-////////////////////////////////////
    // lerpVec3()
    // param 1 - float - lower bound.
    // param 2 - float - higher bound.
    // param 3 - float - t. Domain [0, 1].
    static bool lerpVec3(vector3, vector3, float, vector3& outVec);
    static bool add_vec3_to_vector_starting_at_index(std::vector<float>& out);
    
    float const x() const;
    float const y() const;
    float const z() const;

    bool set_x(float _x);
    bool set_y(float _y);
    bool set_z(float _z);
    bool set_to_zero();
        
    bool add_vec3_to_vector(std::vector<float>& out);
};

class tri {
public:
    vector3 points[3];
    vector3 normal;

    tri(vector3 a, vector3 b, vector3 c, vector3 norm);
};

enum class ShapeType : int32_t {
    ShapeType = -100,
    INVALID = -1,
    TRI = 0,
    RECTANGLE,
    CIRCLE,
    CUBE,
    COUNT
};

enum class VertexDataType {
    VertexDataType = -100,
    INVALID = -1,
    POS = 0,
    NORM = 1,
    COLOR = 2,
    COUNT
};

//-///////////////////////////////////////////////////
// Macro data about a shape
// Shape Data - the shape we want to generate. 
//      Type - (pyramid, sphere, cube, etc.)
//      Metrics - (array of floats. 1-3 in  size.). Used differently based on type. 
//                  i.e. radius (1), base/height (2), base/height/depth (3)
struct shapeConfig {
    ShapeType type = ShapeType::INVALID;
    std::vector<float> sizeMetrics = { } ; 
    shapeConfig(ShapeType _type, std::vector<float> _sizeMetrics);
};

struct vertexConfig {
    // Data for each vertex. 
    std::vector<VertexDataType> vertexDataTypes = { VertexDataType::POS }; // data types packed into each vertex
    std::vector<unsigned int> dataSizes; // number of floats that should be allocated per data type. 
    vertexConfig(std::vector<VertexDataType> _vertexDataTypes, std::vector<unsigned int> _dataSizes);
};

struct shapeConstructionParams {
    shapeConfig shapeConfig;
    vertexConfig vertexConfig;
    shapeConstructionParams(struct shapeConfig _shapeConfig, struct vertexConfig _vertexConfig);
};

//-////////////////////////////////////
// Model data.
// VAO, VBO, EBO, and vertex count. 
struct modelBuffer {
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int verticesCount = 0;
    
    modelBuffer(unsigned int _VAO,
                unsigned int _VBO,
                unsigned int _verticesCount);

    modelBuffer(unsigned int _VAO,
                unsigned int _VBO,
                unsigned int _EBO,
                unsigned int _verticesCount);
};

class modelBufferCache {
public:
    std::vector<modelBuffer> cache;

    bool addModelBuffer(modelBuffer& buf);
};

bool addTriToVector(std::vector<float>& out, tri t);

//-////////////////////////////////////
// Make the triangles for each face of the cube. 
bool buildCubeTris_NoElementBuffer(std::vector<float>& out);

//-////////////////////////////////////
// Make a viewpoint-facing triangle.
bool buildTri(std::vector<float>& out);

// //-////////////////////////////////////
// //
// enum Shape : int32_t
// {
//     Shape = -20,
//     INVALID = -10,
//     TRI = 0,
//     SQUARE = 10,
//     CUBE = 100
// };


// //-////////////////////////////////////
// //
// bool isValidShape(enum Shape s)
// {
//     switch(s)
//     {
//         case Shape::TRI:
//             break;
//         case Shape::SQUARE:
//             break;
//         case Shape::CUBE:
//             break;
//         default:
//             Logging::consoleLog(Logging::LogType::ASSERT, "NOT A VALID SHAPE");
//             return false;
//     }

//     return true;
// }

// bool checkShape()
// {
//     // not implemented
//     return false;
// }
