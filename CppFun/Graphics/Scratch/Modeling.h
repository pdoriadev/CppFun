#ifndef MODELING_H
#define MODELING_H
#endif

#include <vector>
#include <cmath>



// Look into alternative math library for later: GLM. Used and recommended by professor. 
//      OR second choice https://ggt.sourceforge.net/
class vector3
{
public:
    float vec_xyz[3];
    
    vector3();
    vector3(float _x, float _y, float _z);
    
    static bool is_equal(vector3 a, vector3 b, float errorRange);
    static float square_magnitude(vector3 a);
    
    float x();
    float y();
    float z();

    bool set_x(float _x);
    bool set_y(float _y);
    bool set_z(float _z);
    bool set_to_zero();
    
    bool scale_uniform(float s, vector3& outVec);
    bool modulo(int m, vector3& outVec);

    bool add_vec3_to_vector(std::vector<float>& out);
};

class tri
{
public:
    vector3 points[3];
    vector3 normal;

    tri(vector3 a, vector3 b, vector3 c, vector3 norm);
};

struct modelBuffer
{
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int verticesCount = 0;
    
    modelBuffer(unsigned int _VAO,
                unsigned int _VBO,
                unsigned int _verticesCount);
};

class modelBufferCache
{
public:
    std::vector<modelBuffer> cache;

    bool addModelBuffer(modelBuffer& buf);
};

bool addTri(std::vector<float>& out, tri t);

//-////////////////////////////////////
// Make the triangles for each face of the cube. 
bool buildCubeTris(std::vector<float>& out);

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
