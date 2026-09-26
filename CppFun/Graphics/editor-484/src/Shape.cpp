#include "../include/Shape.h"
#include "../include/TextureCache.h"

#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

namespace ShapeMath {
    double const PI() { return M_PI; }
}

#pragma region I/O

bool outputVertices(const std::vector<float>& vertices, uint32_t const VALUES_PER_VERT)
{
    std::string verticesString = "Vertex Data\n";
    verticesString.reserve(vertices.size() * 5);
    for(uint32_t i = 0; i < vertices.size() / VALUES_PER_VERT; ++i)
    {
        uint32_t offsetIndex = i * VALUES_PER_VERT;

        for (uint32_t j = 0; j < VALUES_PER_VERT / 3; ++j) {
            verticesString.append("[");
            verticesString.append(std::to_string(vertices[j*(VALUES_PER_VERT / 3) + offsetIndex]) + ", ");
            verticesString.append(std::to_string(vertices[j*(VALUES_PER_VERT / 3) + offsetIndex+1]) + ", ");
            verticesString.append(std::to_string(vertices[j*(VALUES_PER_VERT / 3) + offsetIndex+2]) + " ");
            verticesString.append("]\t\t");
        }

        verticesString.append("\n");
    }

    std::cout << verticesString << std::endl;

    return true;
}

bool outputElements(const std::vector<uint32_t>& elements, uint32_t const VALUES_PER_ELEMENT)
{
    std::string verticesString = "Elements Data\n";
    verticesString.reserve(elements.size() * 5);

    uint32_t const VALUES_PER_PLANE = VALUES_PER_ELEMENT * 2;
    for(uint32_t i = 0; i < elements.size(); i += VALUES_PER_PLANE)
    {
        for (uint32_t j = 0; j < VALUES_PER_PLANE; j += VALUES_PER_ELEMENT) {
            verticesString.append("[");
            verticesString.append(std::to_string(elements[i + j    ]) + ", ");
            verticesString.append(std::to_string(elements[i + j + 1]) + ", ");
            verticesString.append(std::to_string(elements[i + j + 2]) + " ");
            verticesString.append("]\t\t");
        }

        verticesString.append("\n");
    }

    std::cout << verticesString << std::endl;

    return true;
}

#pragma endregion ==================================================================================================================

std::string getPlaneTypeString(PlaneType type) {
    switch(type) {
        case PlaneType::PLANE_TYPE:         return "PLANE_TYPE";
        case PlaneType::INVALID:            return "INVALID";
        case PlaneType::TOP:                return "TOP";
        case PlaneType::INTERMEDIATE:       return "INTERMEDIATE";
        case PlaneType::WRAP_AROUND_LOOP:    return "WRAP_AROUND_LOOP";
        case PlaneType::BOTTOM:             return "BOTTOM";
        case PlaneType::COUNT:              return "COUNT";
        default: return ""; // invalid type value
    }
}


Shape::Shape(float x, float y, float z, float uniformScale, int colorIndex, int id,
             float scaleX, float scaleY, float scaleZ, bool useUniformScale) 
    // Order follows Shape.h - see the note in Joint.cpp.
    : x(x), y(y), z(z), scale(uniformScale), scaleX(scaleX), scaleY(scaleY), scaleZ(scaleZ),
      useUniformScale(useUniformScale), angleX(0.0f), angleY(0.0f), angleZ(0.0f), colorIndex(colorIndex), id(id),
      texVBO(0),
      defaultX(x), defaultY(y), defaultZ(z),
      defaultScaleX(scaleX), defaultScaleY(scaleY), defaultScaleZ(scaleZ), defaultScale(uniformScale),
      defaultUseUniformScale(useUniformScale),
      defaultRotationX(0.0f), defaultRotationY(0.0f), defaultRotationZ(0.0f),
      defaultColorIndex(colorIndex) {

    // Always initialize, not just for the custom-colour slot. These are plain
    // float arrays, so leaving them unset for preset-coloured shapes meant
    // reading uninitialized memory - harmless while unused, but it wrote
    // garbage like 1.4013e-45 into saved scene files.
    const float initial = (colorIndex == 31) ? 1.0f : 0.0f;
    customColor[0] = initial;  customColor[1] = 0.0f;  customColor[2] = 0.0f;
    defaultCustomColor[0] = initial;
    defaultCustomColor[1] = 0.0f;
    defaultCustomColor[2] = 0.0f;
}

// --- Ray tracing material properties --------------------------------------

void Shape::setReflectivity(float r) { material.reflectivity = glm::clamp(r, 0.0f, 1.0f); }
float Shape::getReflectivity() const { return material.reflectivity; }

void Shape::setTransparency(float t) { material.transparency = glm::clamp(t, 0.0f, 1.0f); }
float Shape::getTransparency() const { return material.transparency; }

void Shape::setIndexOfRefraction(float ior) { material.indexOfRefraction = glm::max(ior, 1.0f); }
float Shape::getIndexOfRefraction() const { return material.indexOfRefraction; }

void Shape::applyMaterial(GLuint shaderProgram) const {
    // Uploaded centrally rather than from each subclass's draw(), so adding a
    // material property does not mean editing ten shape classes.
    GLint loc;
    loc = glGetUniformLocation(shaderProgram, "material.ambient");
    if (loc != -1) glUniform1f(loc, material.ambient);
    loc = glGetUniformLocation(shaderProgram, "material.diffuse");
    if (loc != -1) glUniform1f(loc, material.diffuse);
    loc = glGetUniformLocation(shaderProgram, "material.specular");
    if (loc != -1) glUniform1f(loc, material.specular);
    loc = glGetUniformLocation(shaderProgram, "material.shininess");
    if (loc != -1) glUniform1f(loc, material.shininess);

    // Texture. The hasTexture gate is what keeps untextured rendering
    // byte-identical to what it was before textures existed: with no path set,
    // the sampler is never read and the fragment shader takes exactly the path
    // it took before. raytrace_test's exact image sums depend on that.
    GLuint tex = 0;
    if (!material.texturePath.empty() && hasTexCoords()) {
        tex = TextureCache::handleFor(material.texturePath);
    }

    loc = glGetUniformLocation(shaderProgram, "hasTexture");
    if (loc != -1) glUniform1i(loc, tex != 0 ? 1 : 0);

    if (tex != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        loc = glGetUniformLocation(shaderProgram, "albedoMap");
        if (loc != -1) glUniform1i(loc, 0);
    }
}

glm::vec3 Shape::getBaseColor() const {
    // colorIndex 31 is the "custom color" slot; every other index reads from
    // the shared preset table. This mirrors what each shape's draw() does when
    // it sets the material.color uniform.
    if (colorIndex == 31) {
        return glm::vec3(customColor[0], customColor[1], customColor[2]);
    }
    if (colorIndex < 0 || colorIndex >= 32) {
        return glm::vec3(0.8f, 0.8f, 0.8f);
    }
    return glm::vec3(colorPresets[colorIndex].color[0],
                     colorPresets[colorIndex].color[1],
                     colorPresets[colorIndex].color[2]);
}

// Shape::getModelMatrix() is NOT here either -- it lives in
// src/ShapeTransform.cpp, for the same reason calculateNormals() lives in
// src/ShapeNormals.cpp (see the note further down).
//
// Both are members of this class, declared in Shape.h like everything else.
// Read Shape.h for the whole class in one place; these two definitions sit in
// files of their own only so a solution archive can replace one function
// without replacing all 260 lines of given plumbing in here -- and so the two
// A2 topics, `transforms` and `geometry`, can be swapped independently of each
// other.

// Apply the model matrix to the shader
void Shape::applyTransform(GLuint shaderProgram) const {
    glm::mat4 modelMatrix = getModelMatrix();

    // Pass the model matrix to the shader
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glm::mat4 trans = glm::mat4(1.0f); // creates new identity matrix.
    
    // ROTATE SHAPE TEST
    float const angle = 3.14f * 0.25f * glfwGetTime();
    trans = glm::rotate(trans, angle, glm::vec3(0.77f, 0.77f, 0.0f));
    // ?? WHY AM I PASSING IN THE TRANSPOSE?
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(trans));
    return;

    if (modelLoc != -1) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    } else {
        std::cerr << "Warning: 'model' uniform not found in shader program." << std::endl;
    }
}

void Shape::setRotation(float ax, float ay, float az) {

    // Wrap values between 0 and 360 degrees
    angleX = (ax < 0) ? 360.0f + fmod(ax, 360.0f) : fmod(ax, 360.0f);
    angleY = (ay < 0) ? 360.0f + fmod(ay, 360.0f) : fmod(ay, 360.0f);
    angleZ = (az < 0) ? 360.0f + fmod(az, 360.0f) : fmod(az, 360.0f);

}

void Shape::rotate(float dAngleX, float dAngleY, float dAngleZ) {
    angleX += dAngleX;
    angleY += dAngleY;
    angleZ += dAngleZ;
}

void Shape::setPosition(float nx, float ny, float nz) {
    x = nx;
    y = ny;
    z = nz;
}

void Shape::setScale(float s) {
    scale = s;
    useUniformScale = true;
}

void Shape::setScale(float sx, float sy, float sz) {
    if (sx <= 0.0f || sy <= 0.0f || sz <= 0.0f) {
        std::cerr << "Warning: Scaling values must be positive." << std::endl;
        return;
    }
    scaleX = sx;
    scaleY = sy;
    scaleZ = sz;
    useUniformScale = false;
}

void Shape::useUniformScaling(bool flag) {
    useUniformScale = flag;
    // std::cout << "Scaling mode set to: " << (useUniformScale ? "Uniform" : "Non-Uniform") << std::endl;
}

glm::vec3 Shape::getNonUniformScale() const {
    return glm::vec3(scaleX, scaleY, scaleZ);
}

bool Shape::isUsingUniformScaling() const {
    return useUniformScale;
}

void Shape::setColor(int newColorIndex) {
    colorIndex = newColorIndex;
}

void Shape::setCustomColor(float r, float g, float b) {
    customColor[0] = r;
    customColor[1] = g;
    customColor[2] = b;
}

// Shape::calculateNormals() is NOT here -- it lives in src/ShapeNormals.cpp.
//
// It is a member of this class like everything else in this file; only its
// definition moved. A member function's body can live in any translation unit,
// because Shape.h is what declares it and Shape.h has not changed. What moving
// it buys is that the linker can take it from ONE object file, which is the
// unit a solution archive can swap. See SOLUTION-LIBRARIES.md.


float Shape::getX() const { return x; }
float Shape::getY() const { return y; }
float Shape::getZ() const { return z; }
float Shape::getAngleX() const { return angleX; }
float Shape::getAngleY() const { return angleY; }
float Shape::getAngleZ() const { return angleZ; }
float Shape::getScale() const { return scale; }
int Shape::getColorIndex() const { return colorIndex; }
const float* Shape::getCustomColor() const { return customColor; }
int Shape::getId() const { return id; }
std::string Shape::getShapeType() const { return shapeType; }

void Shape::setShapeType(std::string newShapeType) { shapeType = newShapeType; }

void Shape::addVertex(const glm::vec3 vertex) { vertices.push_back(vertex); }
void Shape::addNormal(const glm::vec3 normal) { normals.push_back(normal); }
void Shape::addFace(int v1, int v2, int v3) { faces.push_back({v1, v2, v3}); }
void Shape::setVertices(const std::vector<glm::vec3>& verts) { vertices = verts; }
void Shape::setNormals(const std::vector<glm::vec3>& norms) { normals = norms; }
void Shape::setTexCoords(const std::vector<glm::vec2>& uvs) { texCoords = uvs; }
void Shape::setFaces(const std::vector<std::vector<int>>& facs) { faces = facs; }


// Implementation of the getVertices method
std::vector<glm::vec3>& Shape::getVertices() {
    return vertices;
}

const std::vector<glm::vec3>& Shape::getVertices() const {
    return vertices;
}

// Implementation of the getFaces method
std::vector<std::vector<int>>& Shape::getFaces() {
    return faces;
}

const std::vector<std::vector<int>>& Shape::getFaces() const {
    return faces;
}

// Implementation of the getNormals method
std::vector<glm::vec3>& Shape::getNormals() {
    return normals;
}

std::vector<glm::vec2>& Shape::getTexCoords() {
    return texCoords;
}

const std::vector<glm::vec2>& Shape::getTexCoords() const {
    return texCoords;
}

// A SECOND VBO on attribute 3, not an extra pair of floats in the existing
// interleaved buffer. Every shape uploads pos3 + normal3 + colour3 with the
// stride hardcoded as 9 * sizeof(float) at 33 sites across nine files;
// widening that to 11 would have touched every one of them, including
// Teapot.cpp at 1642 lines, for no benefit a student can see. A disabled
// attribute 3 reads as a constant, so shapes that never call this are
// unaffected.
void Shape::uploadTexCoords() {
    if (texCoords.empty()) return;

    if (texVBO == 0) glGenBuffers(1, &texVBO);

    glBindBuffer(GL_ARRAY_BUFFER, texVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(texCoords.size() * sizeof(glm::vec2)),
                 texCoords.data(), GL_STATIC_DRAW);

    // Tightly packed vec2s: stride 0 means "same as the element size".
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(3);
}

const std::vector<glm::vec3>& Shape::getNormals() const {
    return normals;
}

// Reset to default
void Shape::resetToDefault() {
    setPosition(defaultX, defaultY, defaultZ);
    setScale(defaultScale);
    setScale(defaultScaleX, defaultScaleY, defaultScaleZ);
    useUniformScaling(defaultUseUniformScale);
    setRotation(defaultRotationX, defaultRotationY, defaultRotationZ);
    setColor(defaultColorIndex);

    if (defaultColorIndex == 31) {  // If custom color, reset it
        setCustomColor(defaultCustomColor[0], defaultCustomColor[1], defaultCustomColor[2]);
    }
}
