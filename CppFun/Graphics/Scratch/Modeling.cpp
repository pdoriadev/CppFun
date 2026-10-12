#include "Modeling.h"
#include "Logging.h" // NOT MULTI-THREAD SAFE
#include <glm/ext/matrix_float2x2.hpp>

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
            return "";
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
    // for (unsigned int i = 0; i < outVertexBufferData.capacity(); ) {
    //     outVertexBufferData.emplace_back(0)

    //     // for each basis vector (i.e x, y, or z). 
    //     for (unsigned int j = 0; j < shapeParams.vertexConfig.dataSizes[i]; ++j) {
            
    //     }

    //     i += vertexElementSize;
    // }

    // for (unsigned int i = 0; i < outVertex)
    


    return true;
}




#pragma endregion =====================================================================================================================
 



/*
Always include an element buffer?



mesh factory functions
- constructs mesh instances based on pre-defined mesh type algorithms and input data (i.e. scale, rotation). 
mesh instance component. 
- instanceID
- Only data for a VAO, VBO, EBO. (i.e. pos, normal, faces. tex coords. etc.)
    - data like pos and normals are stored in parallel arrays. 
- shared ptr. Referenced by mesh renderer components.
    - can be single instance. will stick with shared ptr approach for now.
transform component (POD)
- local pos, quaternion local rotation, local scale
material component???
- references shader code
- material data (textures, color, etc.)

array of all entities
arrays of entity components. Each component has a handle to its entity. 

entity. an index id. Used by the system to index into the entity's components. 
    **Doesn't work when you have hundreds or thousands of components. what then??**

entity manager
- generates and hands out unused entity IDs. Destroys passed in IDs, puts them back in the unused ID collection. 
- manages signatures array. each signature is a bitfield where each bit corresponds to a component. 

component. component is data (i.e. transform data)

system. system operates on data (i.e. transform.move())
    - tracks what entities have components required for the system. ????
    - system updates entity data as needed. 

-----

my use-case

component insertion and deletion functions need to know. Just for functions that work on different types 
of component arrays.
- size of type (parallel arrays of enums and their sizes)
- size of array (count * typeSize)
- index for operation (i.e. insert, delete, etc.)
- entity index if needed.

polymorphism without templates or interfaces. 
- void* for each type. adaptor functions for as needed. 
- sub-type encapsulates the super-type.

functions that operate on arrays of polymorphic component types. 
- void* to array
- size_t of the component type
- enum for type name. 
- indices for operation
- other types. 

*/

class Entity {
public:
    uint const idName;
    Entity(int _idName) : idName(_idName) { }
};

// EntityManager - generates unused entity list. 
class EntityManager {
public:
    std::vector<Entity> queue;
    EntityManager(uint const MAX_SIZE = 10000) {
        queue.reserve(MAX_SIZE);
        for (uint i = 0; i < MAX_SIZE; ++i) {
            queue.emplace_back(i);
        }
    }

    // destructor needs to destroy all components that rely on the entity?

    // create entity


    // destroy entity



};

struct MeshData {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<std::vector<int>> faceIndices;
};

struct TransformData {
    glm::vec3 position;
    glm::mat2 rotation;
    glm::vec3 scale;
};

struct MeshRenderer {
    MeshData* meshData; 
};

enum class ComponentType : int {
    ComponentType = -100,
    INVALID = -1,
    TRANSFORM = 0,
    MESH = 1
};

std::string getComponentTypeString(ComponentType type) {
    switch(type) {
        case ComponentType::TRANSFORM:      return "TRANSFORM";
        case ComponentType::MESH:           return "MESH_RENDERER";
        case ComponentType::INVALID:        return "INVALID";
        case ComponentType::ComponentType:  return "ComponentType";
        default:
            Logging::consoleLog(Logging::LogType::ASSERT, "Unknown Component type passed.");
            return "Unknown Type";
    }
}

// This system is only used to manipulate the components array. 
// This multi-enum approach to extract should not be used elsewhere. 
// returns -1 when an invalid type is passed into the function. 
int const getComponentTypeSize(ComponentType type) {
    switch(type) {
        case ComponentType::TRANSFORM:      return sizeof(TransformData);
        case ComponentType::MESH:           return sizeof(MeshRenderer);
        default:
            Logging::consoleLog(Logging::LogType::ASSERT, getComponentTypeString(type) + " does not have a component size implementation.");
            return -1;
    }
}
// populate at start of runtime???? bleh. 
// once you have a component's index, you can get all the component's data in any other array. 

std::vector<ComponentType> componentTypes;
std::vector<std::size_t> componentSizes;
std::vector<std::string_view> componentNames;

// array of component types. 
// parallel arrays for each 

ComponentType const getComponentType(ComponentType type);

// what type does the component array hold?

std::vector<TransformData> transforms;
std::vector<MeshData> meshes;


/* Implementing dynamic void* arrays for components. 
-- Why do this --
templates are annoying. increase compilation time. 
    --> but you haven't made a template before? How annoying are they?
I'm trying to make this section hyper-performant. interfaces will introduce vp's with indirection
    --> vp's and the vtable are not true bottlenecks. You don't know enough to know. 
        They will have some indirection. but you don't know. 
I want to understand how to use memory like a sports car. Feels good.

I want to understand how to do this for possible embedded direction. 

I want to intimately understand how dynamic arrays function. Will give me an understanding for memory and how it is used in other domains. 


void* is a way to store a sequence of any type in a contiguous block. 

------
void* does not actually point to a memory location. Its bytes store data for a given type. 


-- walk through usage --
construct array object. Its type and type size must be known at construction. 





-- allocation --
If reaches max size on an expanding operation, 
    - allocate new place in memory. 
    - copy pointers over
    - 
- al

-- functions --
sizeInBytes();
type();
elementSize();
reserve(); - mallocs space



-- variables --
std::size_t const elementSize
enum const type;
size_t length;








*/


struct ComponentArrayReference {
    // pointer to components array
    std::vector<void*> &componentsArray;
    ComponentType const type;
    
    ComponentArrayReference(std::vector<void*> &_componentsArray, ComponentType const _type)
        : componentsArray(_componentsArray), type(_type) { 
            validate(); 
        }

    bool validate() {
        bool success = true;
        std::string componentStr = getComponentTypeString(type);
        std::string errorLogMessage = "Failed to validate component array of type " + getComponentTypeString(type);
        
        if (getComponentTypeSize(type) < 0) {
            success = false;
            errorLogMessage += "\nInvalid component type.";
        }

        if (success == false) {
            Logging::consoleLog(Logging::LogType::ASSERT, errorLogMessage);
            return false;
        }

        Logging::consoleLog(Logging::LogType::LOG, "Successfully validated component array reference of type " + componentStr);
        return true;
    }
};


bool addComponent(ComponentArrayReference arrRef, Entity entity) {
    // inserting component with a raw c array sounds like a massssssssive pain. 
    std::size_t const size = getComponentTypeSize(arrRef.type);

    

    return true;
}


/*

------

what is a type?
A type is a contract. 
explicitly defines a label, a byte size, and rules for that type (i.e. int vs. uint)

struct type { data };
returnType func funcForType(func that initializes, operates on, reads, or destroys type);

structs bundle data together for specific use cases. 
THIS IS BAD. NO GOOD. BAD BAD. HAPPY NO. NUH UH. 




------
parallel arrays for all components. each array equals the entity count total. 
index into each arrays with the entity id. Check if it has the components for a given system?

construction algorithm. function pointer?
always construct triangles. 


----






*/