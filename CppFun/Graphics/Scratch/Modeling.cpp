#include "Modeling.h"

#pragma region vector3

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

float vector3::x() { return vec_xyz[0]; }
float vector3::y() { return vec_xyz[1]; }
float vector3::z() { return vec_xyz[2]; }

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

#pragma endregion =====================================================================================================================

#pragma region MODEL_BUFFER

modelBuffer::modelBuffer(unsigned int _VAO,
                unsigned int _VBO,
                unsigned int _verticesCount)
{
    VAO = _VAO;
    VBO = _VBO;
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

bool addTri(std::vector<float>& out, tri t) {
    for (unsigned int i = 0; i < 3; ++i)
    {
        t.points[i].add_vec3_to_vector(out);
        t.normal.add_vec3_to_vector(out);
    }

    return true;
}

//-////////////////////////////////////
// Make the triangles for each face of the cube. 
bool buildCubeTris(std::vector<float>& out) {
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
    addTri(out, tri(zero, one, six, backNorm));
    addTri(out, tri(zero, seven, six, backNorm));

    // Top Face
    addTri(out, tri(zero, one, two, topNorm));
    addTri(out, tri(zero, three, two, topNorm));

    // Front Face
    addTri(out, tri(two, three, four, frontNorm));
    addTri(out, tri(two, five, four, frontNorm));

    // Bottom Face
    addTri(out, tri(four, five, six, botNorm));
    addTri(out, tri(four, seven, six, botNorm));

    // sideA Face
    addTri(out, tri(zero, three, four, sideANorm));
    addTri(out, tri(zero, seven, four, sideANorm));

    // sideB Face
    addTri(out, tri(one, two, five, sideBNorm));
    addTri(out, tri(one, six, five, sideBNorm));

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

    addTri(out, tri(zero, one, two, norm));

    return true;
}