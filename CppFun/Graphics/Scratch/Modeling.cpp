#include "Modeling.h"
#include "Logging.h" // NOT MULTI-THREAD SAFE

#pragma region VECTOR3

vector3::vector3() {
    vec_xyz[0] = 0.0;
    vec_xyz[1] = 0.0;
    vec_xyz[2] = 0.0;
};

vector3::vector3(float _x, float _y, float _z) {
    vec_xyz[0] = _x;
    vec_xyz[1] = _y;
    vec_xyz[2] = _z;
};

/*static*/ vector3 const vector3::vec_zero(0.0f, 0.0f, 0.0f);

float const vector3::x() const { return vec_xyz[0]; }
float const vector3::y() const { return vec_xyz[1]; }
float const vector3::z() const { return vec_xyz[2]; }

bool vector3::set_x(float _x) { vec_xyz[0] = _x; return true;}
bool vector3::set_y(float _y) { vec_xyz[1] = _y; return true;}
bool vector3::set_z(float _z) { vec_xyz[2] = _z; return true;}
bool vector3::set_to_zero() { set_x(0.0f); set_y(0.0f); set_z(0.0f); return true;}

/*static*/ float vector3::square_magnitude(vector3 a) {
    return (a.x()*a.x() + a.y()*a.y() + a.z()*a.z()); 
}

/*static*/ bool vector3::is_equal(vector3 a, vector3 b, float errorRange) {   
    a.set_x(a.x() - b.x());
    a.set_y(a.y() - b.y());
    a.set_z(a.z() - b.z());

    if (square_magnitude(a) > errorRange) { return false;}

    return true;
}

/*static*/ bool vector3::add_first_to_second(vector3 a, vector3& outVec) {
    outVec.set_x(a.x() + outVec.x());
    outVec.set_y(a.y() + outVec.y());
    outVec.set_z(a.z() + outVec.z());

    return true;
}

/*static*/ vector3 vector3::add(vector3 a, vector3 b) {
    vector3 sum = vector3(  a.x() + b.x(),
                            a.y() + b.y(),
                            a.z() + b.z());
    return sum;
}

/*static*/ vector3 vector3::scale_uniform(float s, vector3 vec) {
    vec.set_x(s * vec.x());
    vec.set_y(s * vec.y());
    vec.set_z(s * vec.z());
    return vec;
}

/*static*/ bool vector3::modulo(int m, vector3& outVec) {
    outVec.set_x(fmodf(outVec.x(), static_cast<float>(m)));
    outVec.set_y(fmodf(outVec.y(), static_cast<float>(m)));
    outVec.set_z(fmodf(outVec.z(), static_cast<float>(m)));
    return true;
}

/*static*/ bool vector3::lerpVec3(vector3 lowerBound, vector3 upperBound, float t, vector3& outVec) {
    if (t < 0 || t > 1) { return false;}

    vector3 tTimesBminusA = scale_uniform(t, 
                add(upperBound, scale_uniform(-1.0f, lowerBound))
                );
    outVec = add(lowerBound, tTimesBminusA);

    return true;
}

/*static*/ bool add_vec3_to_vector_starting_at_index(vector3 const vec3, 
                                                    unsigned int const starting_index, 
                                                    std::vector<float>& outVec) {
    outVec.emplace(outVec.begin() + starting_index, vec3.x());
    outVec.emplace(outVec.begin() + starting_index + 1, vec3.y());
    outVec.emplace(outVec.begin() + starting_index + 2, vec3.z());
    return true;
}

bool vector3::add_vec3_to_vector(std::vector<float>& out) {
    out.emplace_back(x());
    out.emplace_back(y());
    out.emplace_back(z());
    return true;
}



#pragma endregion =====================================================================================================================

#pragma region TRI_FUNCTIONS

tri::tri(vector3 a, vector3 b, vector3 c, vector3 norm)
{
    points[0] = a;
    points[1] = b;
    points[2] = c;
    normal = norm;
}

bool addTriToVector(std::vector<float>& out, tri t) {
    for (unsigned int i = 0; i < 3; ++i)
    {
        t.points[i].add_vec3_to_vector(out);
        t.normal.add_vec3_to_vector(out);
    }

    return true;
}

// I DON'T KNOW HOW TO DO THIS WITH AN ELEMENT BUFFER OBJECT YET. WHERE DO I STORE THE NORMAL DATA??????
//-////////////////////////////////////
// Make the triangles for each face of the cube. 
// bool buildCubeTris_Indexed(std::vector<float>& outVertices, std::vector<unsigned int>& outIndices) {
 
//     vector3 zero        (0.5f,     0.5f,   -0.5f);
//     vector3 one         (-0.5f,    0.5f,   -0.5f);
//     vector3 two         (-0.5f,    0.5f,   0.5f);
//     vector3 three       (0.5f,     0.5f,   0.5f);
//     vector3 four        (0.5f,     -0.5f,  0.5f);
//     vector3 five        (-0.5f,    -0.5f,  0.5f);
//     vector3 six         (-0.5f,    -0.5f,  -0.5f);
//     vector3 seven       (0.5f,     -0.5f,  -0.5f);
//     outVertices.reserve(8);
//     zero.add_vec3_to_vector(outVertices) ;   

//     static vector3 const sideANorm   (1.0f,     0,      0);
//     static vector3 const topNorm     (0.0f,     1.0f,   0);
//     static vector3 const frontNorm   (0.0f,     0,      1.0f);
//     static vector3 const backNorm    (0.0f,     0,      -1.0f);
//     static vector3 const botNorm     (0.0f,     -1.0f,  0);
//     static vector3 const sideBNorm   (-1.0f,    0,      0);

//     // tri vertices are always added by going around the outside of the square. Never across the square. 

//     // Back Face
//     addTriToVector(outVertices, tri(zero, one, six, backNorm));
//     addTriToVector(outVertices, tri(zero, seven, six, backNorm));

//     // Top Face
//     addTriToVector(outVertices, tri(zero, one, two, topNorm));
//     addTriToVector(outVertices, tri(zero, three, two, topNorm));

//     // Front Face
//     addTriToVector(outVertices, tri(two, three, four, frontNorm));
//     addTriToVector(outVertices, tri(two, five, four, frontNorm));

//     // Bottom Face
//     addTriToVector(outVertices, tri(four, five, six, botNorm));
//     addTriToVector(outVertices, tri(four, seven, six, botNorm));

//     // sideA Face
//     addTriToVector(outVertices, tri(zero, three, four, sideANorm));
//     addTriToVector(outVertices, tri(zero, seven, four, sideANorm));

//     // sideB Face
//     addTriToVector(outVertices, tri(one, two, five, sideBNorm));
//     addTriToVector(outVertices, tri(one, six, five, sideBNorm));

//     return true; 
// }

//-////////////////////////////////////
// Make the triangles for each face of the cube. 
bool buildCubeTris_NoElementBuffer(std::vector<float>& out) {
    static vector3 const zero        (0.5f,     0.5f,   -0.5f);
    static vector3 const one         (-0.5f,    0.5f,   -0.5f);
    static vector3 const two         (-0.5f,    0.5f,   0.5f);
    static vector3 const three       (0.5f,     0.5f,   0.5f);
    static vector3 const four        (0.5f,     -0.5f,  0.5f);
    static vector3 const five        (-0.5f,    -0.5f,  0.5f);
    static vector3 const six         (-0.5f,    -0.5f,  -0.5f);
    static vector3 const seven       (0.5f,     -0.5f,  -0.5f);

    static vector3 const sideANorm   (1.0f,     0,      0);
    static vector3 const topNorm     (0.0f,     1.0f,   0);
    static vector3 const frontNorm   (0.0f,     0,      1.0f);
    static vector3 const backNorm    (0.0f,     0,      -1.0f);
    static vector3 const botNorm     (0.0f,     -1.0f,  0);
    static vector3 const sideBNorm   (-1.0f,    0,      0);

    // tri vertices are always added by going around the outside of the square. Never across the square. 

    // Back Face
    addTriToVector(out, tri(zero, one, six, backNorm));
    addTriToVector(out, tri(zero, seven, six, backNorm));

    // Top Face
    addTriToVector(out, tri(zero, one, two, topNorm));
    addTriToVector(out, tri(zero, three, two, topNorm));

    // Front Face
    addTriToVector(out, tri(two, three, four, frontNorm));
    addTriToVector(out, tri(two, five, four, frontNorm));

    // Bottom Face
    addTriToVector(out, tri(four, five, six, botNorm));
    addTriToVector(out, tri(four, seven, six, botNorm));

    // sideA Face
    addTriToVector(out, tri(zero, three, four, sideANorm));
    addTriToVector(out, tri(zero, seven, four, sideANorm));

    // sideB Face
    addTriToVector(out, tri(one, two, five, sideBNorm));
    addTriToVector(out, tri(one, six, five, sideBNorm));

    return true; 
}

//-////////////////////////////////////
// Make a viewpoint-facing triangle.
bool buildTri(std::vector<float>& out) {
    // origin at bottom center
    static const vector3 zero       (-0.5, 0, 0);
    static const vector3 one        (0.5, 0, 0);
    static const vector3 two        (0, 1,0);

    static const vector3 norm       (0, 0, 1);

    addTriToVector(out, tri(zero, one, two, norm));

    return true;
}

#pragma endregion =====================================================================================================================

#pragma region SHAPE_VERTEX_CONFIG

vertexConfig::vertexConfig(std::vector<VertexDataType> _vertexDataTypes, std::vector<unsigned int> _dataSizes) 
: vertexDataTypes{_vertexDataTypes}, dataSizes{_dataSizes} { }

shapeConstructionParams::shapeConstructionParams(struct shapeConfig _shapeConfig, struct vertexConfig _vertexConfig) 
: shapeConfig{_shapeConfig}, vertexConfig{_vertexConfig} { }

#pragma endregion

#pragma region MODEL_BUFFER

modelBuffer::modelBuffer(unsigned int _VAO,
                        unsigned int _VBO,
                        unsigned int _verticesCount)
{
    VAO = _VAO;
    VBO = _VBO;
    verticesCount = _verticesCount;
}

modelBuffer::modelBuffer(unsigned int _VAO,
                        unsigned int _VBO,
                        unsigned int _EBO,
                        unsigned int _verticesCount){
    VAO = _VAO;
    VBO = _VBO;
    EBO = _EBO;
    verticesCount = _verticesCount;
}

#pragma endregion =====================================================================================================================

#pragma region MODEL_BUFFER_CACHE

bool modelBufferCache::addModelBuffer(modelBuffer& buf)
{
    cache.emplace_back(buf);
    return true;
}

#pragma endregion =====================================================================================================================


// algorithms for new pos, normal, and color data. 
// algorithms for modifying that data.
// Order them in glm::vec3's. or vec 4's (i.e. for color)
// Each vertex's data is contiguous in the VBO. 
// When the EBO references one element in the VBO, it grabs all that data. 
// enum class for each GLSL data type? or GLFW? or glm? 
//      Keeping it to one library would be nice. Leaning towards GLFW because that's what I'm using everywhere else.
// array of function pointers. Each fp points to a function for adding data to a vertex.
    // pass an array of data types. 
    // Each element in the array corresponds to a data type to add to the VBO.
// But what are the data values?
// const or procedural. 
    // const - pass reference to vector of values.
    // procedural - pass enum that maps to a specific function. 
// how to determine const vs procedural for each piece of data?
//      Array of VertexData structs. 
//          VertexData - data about a single vertex. 
//              enum dataType // color, position, normal, etc.
//              uint dataSize // # of elements in the data array. 0 if procedural. 
// All vertices will have the same data. SO, I know where each data type should be allocated
//      before I start building. 
// What if I want to assign a specific color to a single vertex?
// I think, for now, at least, I've got to do this fully static data or fully procedural. 
//      Doing partially procedural or partially static means making assumptions about how they 
//      connect. I can't make assumptions, because I haven't done procedural before. 
// 


#pragma CREATE_VBO_DATA

std::string getShapeVertexString(ShapeType const type) {
    switch(type) {
        case ShapeType::ShapeType:      return "ShapeType";
        case ShapeType::INVALID:        return "INVALID";
        case ShapeType::TRI:            return "TRI";
        case ShapeType::RECTANGLE:      return "RECTANGLE";
        case ShapeType::CIRCLE:         return "CIRCLE";
        case ShapeType::CUBE:           return "CUBE";
        case ShapeType::COUNT:          return "COUNT";
        default:
            Logging::consoleLog(Logging::LogType::ERROR, "Missing implementation for switch case.");
    }
}

bool getShapeVertexCount(ShapeType const type, unsigned int& size) {
    switch(type) {
        case ShapeType::TRI:
            size = 3; break;
        case ShapeType::RECTANGLE:
            size = 4; break;
        case ShapeType::CUBE:
            size = 8; break;
        default:
            Logging::consoleLog(Logging::LogType::ASSERT, 
                "Invalid shape type: " + getShapeVertexString(type));
            return false;
    }

    return true;
}

// bool constructTri(shapeConstructionParams shapeParams,
//                     std::vector<float>& outVertexBufferData,
//                     std::vector<unsigned int>& outElementBufferData,
//                     unsigned int vertexElementSize) {
//     vector3 vert (0, 0, 0);
//     add_vec3_to_vector_starting_at_index(vert, 0, outVertexBufferData);
//     add_vec3_to_vector_starting_at_index(vert, 1 * vertexElementSize , outVertexBufferData);
//     add_vec3_to_vector_starting_at_index(vert, 2 * vertexElementSize, outVertexBufferData);
    
//     return true;
// }

// FULLY PROCEDURAL - generates all vertex data into buffer variables before binding. 
// array* of data types. (POS, COLOR, NORM)
// array* of data sizes. (1, 3, 4)
//
// out vector reference. Where we assign VBO data. 
// out vector reference. Where we assign EBO data. Tells VAO what indices make each triangle.
//      
// calculate N number of vertex positions based on shape data. 
// reserve space in VBO = 
// set position data.
//      calls function matching shape type. 
//      assign EBO indices while this is happening
// set other data types. Call function that 3matches data types. 
//      array of all data types 
//      array of function pointers.
/*
OUTER LOOP
- Inspect Vertex Data element. 
    INNER LOOP
*/
bool constructShapeBuffer(shapeConstructionParams shapeParams,
                        std::vector<float>& outVertexBufferData,
                        std::vector<unsigned int>& outElementBufferData) {
    unsigned int vertexCount;
    if (getShapeVertexCount(shapeParams.shapeConfig.type, 
                            vertexCount) == false) return false;
    
    // Element size = Sum(N, M)
    // N = number of data types
    // M = size of each data type
    unsigned int vertexElementSize;
    for (unsigned int i; i < shapeParams.vertexConfig.dataSizes.size(); ++i) {
        vertexElementSize += shapeParams.vertexConfig.dataSizes[i];
    }

    outVertexBufferData.reserve(vertexCount * vertexElementSize);

    // go through all combinations in a 2x2 matrix of 0's and 1's. 
    // 0 = [ 0, 0, 0 ], 1 = [ 0, 1, 0 ], 2 = [ 1, 0, 0 ], 3 = [1, 1, 0]
    // 4 = [ 0, 0, 1 ], 5 = [ 0, 1, 1 ], 6 = [ 1, 0, 1 ], 7 = [1, 1, 1]
    // 3 variables. 8 possible vertices. 2^3 = 8. 
    // How to indices in the correct order to the element buffer. It doesn't *technically* matter
    // how they are arranged in the VBO, althought it may help with cache coherency? who gives a shit. just get something working.
    // pick a plane that stays on one axis. . Then add the vertices in the clock-wise direction of that plane.
    // The above indices are described wrt to the z plane not moving. counter clock-wise always goes positive horiz, pos up, neg horiz, neg down. 
    // PosH, PosV, NegH, NegV. Horizontal and Vertical is wrt the plane *NOT* world or local transform.
    // selecting a triangle's vertices: 
    //                  constrain to a plane s.t. one of the x, y, or z basis vectors is always 0 or 1. 
    //                  First triangle - find vertices whose difference in position is posHoriz, then posVert.
    //                  Second triangle - start with the last triangle's vertex. find vertices whose difference in position is negHoriz, then negVert.
    // z- | 0, 1, 2;  2, 3, 0;      1, 2, 3;   3, 0, 1;
    // z+ | 5, 4, 7;  7, 6, 5;      4, 7, 6;   6, 5, 4;
    // x+ | 1, 5, 6;  6, 2, 1;      5, 6, 2;   2, 1, 5;
    // x- | 4, 0, 3;  3, 7, 4;      0, 3, 7;   7, 4, 0;
    // y+ | 3, 2, 6;  6, 7, 3;      2, 6, 7;   7, 3, 2;
    // y- | 0, 4, 5;  5, 1, 0;      1, 0, 4;   4, 5, 1; <-- this one. PosPos. NegNeg. 
    //
    // a cube forms 6 face loops. Each loop edge is equal to any two orthogonal vertices 
    // Derive the index for the next vertex through bit shifting???????


    switch(shapeParams.shapeConfig.type) {
        case ShapeType::TRI:
            for (unsigned int i = 0; i < vertexCount; ++i) {
                
                add_vec3_to_vector_starting_at_index(vector3(0,0,0), 0, outVertexBufferData);

            }
            // add position for each vertex
            add_vec3_to_vector_starting_at_index(vector3(0,1,0), 1 * vertexElementSize, outVertexBufferData);
            add_vec3_to_vector_starting_at_index(vector3(1,1,0), 2 * vertexElementSize, outVertexBufferData);
            // add positions to element buffer
            outElementBufferData.emplace_back(0);
            
            // add normal for each vertex
            for (unsigned int i = 0; i < 3; ++i){
                add_vec3_to_vector_starting_at_index(vector3(0,0,-1.0f), i * vertexElementSize, outVertexBufferData);
            }
            break;
        default:
            Logging::consoleLog(Logging::LogType::ASSERT, 
                getShapeVertexString(ShapeType::ShapeType) + 
                " '" + getShapeVertexString(shapeParams.shapeConfig.type) + "' is not a valid case.");
            return false;
    }

    // TODO replace with switch case that calls appropriate shape function. 
    // Add positions
    for (unsigned int i = 0; i < outVertexBufferData.capacity(); ) {
        outVertexBufferData.emplace_back(0)

        // for each basis vector (i.e x, y, or z). 
        for (unsigned int j = 0; j < shapeParams.vertexConfig.dataSizes[i]; ++j) {
            
        }

        i += vertexElementSize;
    }

    for (unsigned int i = 0; i < outVertex)
    


    return true;
}




#pragma endregion =====================================================================================================================
 
