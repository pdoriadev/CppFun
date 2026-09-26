#ifndef SHAPE_H
#define SHAPE_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> // glm core. 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <vector>
#include <string>
#include "ColorPresets.h"
#include "Globals.h"
#include "Material.h"


#define _USE_MATH_DEFINES 
#include <cmath>

namespace ShapeMath {
    double const PI();
}

enum class PlaneType : int32_t {
    PLANE_TYPE = -100,
    INVALID = -1,
    TOP = 0,
    INTERMEDIATE,
    WRAP_AROUND_LOOP,
    BOTTOM,
    COUNT
};

std::string getPlaneTypeString(PlaneType type);
bool outputVertices(const std::vector<float>& vertices, uint32_t const VALUES_PER_VERT);
bool outputElements(const std::vector<uint32_t>& elements, uint32_t const VALUES_PER_ELEMENT);

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

class Shape {

public:
    Shape(float x, float y, float z, float uniformScale, int colorIndex, int id, 
          float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f, bool useUniformScale = true);
    virtual ~Shape() = default;

    virtual void draw(GLuint shaderProgram) = 0;

    // Transformation-related methods
    void setRotation(float ax, float ay, float az);
    void rotate(float dAngleX, float dAngleY, float dAngleZ);
    void setPosition(float nx, float ny, float nz);

    void setScale(float s); // Uniform scale
    void setScale(float sx, float sy, float sz); // Non-uniform scale
    void useUniformScaling(bool flag); // Toggle scaling mode

    // Apply transformations using shaders
    void applyTransform(GLuint shaderProgram) const;

    // Color-related methods
    void setColor(int newColorIndex);
    void setCustomColor(float r, float g, float b);

    // Calculate normal values
    void calculateNormals();
    
    // Getter methods
    float getX() const;
    float getY() const;
    float getZ() const;

    float getAngleX() const;
    float getAngleY() const;
    float getAngleZ() const;

    float getScale() const;
    glm::vec3 getNonUniformScale() const;
    bool isUsingUniformScaling() const;

    int getColorIndex() const;
    const float* getCustomColor() const;
    int getId() const;
    std::string getShapeType() const;

    // Set the shape type
    void setShapeType(std::string newShapeType);

    bool constructPlane(PlaneConstructionParams);
    
    void addVertex(const glm::vec3 vertex);
    void addNormal(const glm::vec3 normal);
    void addFace(int v1, int v2, int v3);
    void setVertices(const std::vector<glm::vec3>& verts);
    void setNormals(const std::vector<glm::vec3>& norms);
    void setTexCoords(const std::vector<glm::vec2>& uvs);
    void setFaces(const std::vector<std::vector<int>>& facs);

    // Add these methods to allow access to vertices, faces, and normals
    std::vector<glm::vec3>& getVertices();
    const std::vector<glm::vec3>& getVertices() const;

    std::vector<std::vector<int>>& getFaces();
    const std::vector<std::vector<int>>& getFaces() const;

    std::vector<glm::vec3>& getNormals();
    const std::vector<glm::vec3>& getNormals() const;
    std::vector<glm::vec2>& getTexCoords();
    const std::vector<glm::vec2>& getTexCoords() const;
    bool hasTexCoords() const { return !texCoords.empty(); }

    // Uploads texCoords as a second VBO on attribute 3 and enables it. Call
    // from a subclass's setupShape() with its VAO bound. Does nothing when
    // texCoords is empty, so a shape that has not been given a
    // parameterisation is untouched.
    void uploadTexCoords();

    // The UV buffer handle, or 0 if uploadTexCoords() was never called.
    //
    // Exposed for one reason: a shape can generate a perfectly correct
    // texCoords array and simply forget to upload it, which leaves attribute 3
    // disabled. GL then feeds a constant, every fragment samples one texel, and
    // the shape renders as a flat block of whatever colour sits at (0,0) --
    // while every CPU-side check, and the entire ray tracer, still pass.
    GLuint getTexVBO() const { return texVBO; }

    // Reset to default values
    void resetToDefault();

    // --- Ray tracing support ---------------------------------------------
    // Public because the ray tracer and the mouse picker need to place this
    // shape's geometry in world space. It used to be protected, when only
    // applyTransform() consumed it.
    glm::mat4 getModelMatrix() const;

    // Resolved RGB base color, applying the same colorIndex == 31 "custom
    // color" rule the draw() methods use, so traced images match the viewport.
    glm::vec3 getBaseColor() const;

    // How many ints each vertex occupies inside an entry of `faces`.
    //
    // Most shapes store plain vertex indices, so faces[f] = {v0, v1, v2} and
    // the stride is 1. ImportShape instead interleaves vertex and normal
    // indices - faces[f] = {v0, n0, v1, n1, v2, n2} - so its stride is 2.
    // Anything reading `faces` generically (the ray tracer, the picker) must
    // honour this or it will read normal indices as vertex indices and produce
    // scrambled geometry.
    virtual int getFaceIndexStride() const { return 1; }

    // Full surface description. Defaults reproduce the previous fixed shading.
    Material&       getMaterial()       { return material; }
    const Material& getMaterial() const { return material; }
    void setMaterial(const Material& m) { material = m; }
    void resetMaterial()                { material = Material(); }

    // Uploads ka/kd/ks/shininess. Called once per shape by the render loop,
    // just before draw(), which still supplies material.color itself.
    void applyMaterial(GLuint shaderProgram) const;

    // Convenience accessors kept so existing call sites keep compiling.
    void  setReflectivity(float r);
    float getReflectivity() const;
    void  setTransparency(float t);
    float getTransparency() const;
    void  setIndexOfRefraction(float ior);
    float getIndexOfRefraction() const;

    // False for things that should be lit but never block light or appear in a
    // render - light gizmos, for instance.
    virtual bool providesRayGeometry() const { return true; }

    // Stable identifier written to scene files.
    //
    // Deliberately NOT shapeType: the importers overwrite shapeType with the
    // source filename ("Bunny", "Teapot-mid"), so it cannot identify a class.
    virtual const char* serialType() const { return "Shape"; }

    // Set for shapes reconstructed by re-reading an external file. Stored in
    // the scene relative to the scene file where possible, so a scenes/ folder
    // stays portable.
    const std::string& getSourcePath() const { return sourcePath; }
    void setSourcePath(const std::string& p) { sourcePath = p; }


protected:
    float x, y, z;
    float scale;
    float scaleX, scaleY, scaleZ; // Non-uniform scaling along axes
    bool useUniformScale;     // Flag to toggle between scaling modes
    float angleX, angleY, angleZ;
    int colorIndex;
    float customColor[3];
    int id;
    std::string shapeType;

    // Surface properties
    Material material;

    std::string sourcePath;   // empty unless loaded from a file

    // Vertices, normals, and faces
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    // Texture coordinates, one per vertex, uploaded as a SECOND VBO bound to
    // attribute 3 rather than interleaved into the stride-9 buffer every shape
    // already uses. Interleaving would have meant editing 33 call sites across
    // nine files including Teapot.cpp (1642 lines) and would have forced every
    // shape to produce UVs whether it has a sensible parameterisation or not.
    // Empty here means "no UVs": attribute 3 stays disabled and reads as a
    // constant, and the hasTexture uniform gates the sampler anyway.
    std::vector<glm::vec2> texCoords;
    GLuint texVBO;      // 0 until uploadTexCoords() runs
    std::vector<std::vector<int>> faces;

private:

    // Default values for reset
    float defaultX, defaultY, defaultZ;
    float defaultScaleX, defaultScaleY, defaultScaleZ;
    float defaultScale;
	bool defaultUseUniformScale;
    float defaultRotationX, defaultRotationY, defaultRotationZ;
    int defaultColorIndex;
    float defaultCustomColor[3];  // For custom color if used    
    
};

#endif
