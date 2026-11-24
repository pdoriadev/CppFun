#ifndef LIGHT_H
#define LIGHT_H

#include "Shape.h"

// A light source that lives in the scene graph alongside the shapes.
//
// Deriving from Shape is deliberate: it means a light gets selection, a
// position, an entry in the Shape Selector and the properties panel for free,
// exactly like any other object. What it must NOT do is behave like geometry -
// a light should never block another light or show up as a surface in a render.
// providesRayGeometry() returns false for precisely that reason; the small
// octahedral gizmo it carries exists only so you can see and click it in the
// viewport.
class Light : public Shape {
public:
    const char* serialType() const override { return "Light"; }
    enum LightType {
        Point       = 0,
        Directional = 1
    };

    Light(float x, float y, float z, int id);
    ~Light();

    void draw(GLuint shaderProgram) override;

    // Never occludes, never appears as a surface in a traced image.
    bool providesRayGeometry() const override { return false; }

    // --- attributes -------------------------------------------------------
    int  getType() const { return type; }
    void setType(int t)  { type = t; }

    bool isEnabled() const { return enabled; }
    void setEnabled(bool e) { enabled = e; }

    const glm::vec3& getColor() const { return color; }
    void setColor(const glm::vec3& c) { color = c; }

    float getIntensity() const { return intensity; }
    void  setIntensity(float i) { intensity = (i < 0.0f) ? 0.0f : i; }

    float getAmbientScale()  const { return ambientScale; }
    float getDiffuseScale()  const { return diffuseScale; }
    float getSpecularScale() const { return specularScale; }
    void  setAmbientScale(float v)  { ambientScale = v; }
    void  setDiffuseScale(float v)  { diffuseScale = v; }
    void  setSpecularScale(float v) { specularScale = v; }

    // Direction a directional light travels in. Ignored for point lights.
    const glm::vec3& getDirection() const { return direction; }
    void setDirection(const glm::vec3& d) { direction = d; }

    // Distance falloff for point lights: 1 / (c + l*d + q*d*d).
    // Defaults to (1, 0, 0), i.e. no falloff, matching the previous behaviour.
    float getConstantAttenuation()  const { return constantAtt; }
    float getLinearAttenuation()    const { return linearAtt; }
    float getQuadraticAttenuation() const { return quadraticAtt; }
    void  setConstantAttenuation(float v)  { constantAtt = v; }
    void  setLinearAttenuation(float v)    { linearAtt = v; }
    void  setQuadraticAttenuation(float v) { quadraticAtt = v; }

    // --- resolved contributions, shared by rasterizer and ray tracer -------
    glm::vec3 effectiveAmbient()  const { return color * intensity * ambientScale; }
    glm::vec3 effectiveDiffuse()  const { return color * intensity * diffuseScale; }
    glm::vec3 effectiveSpecular() const { return color * intensity * specularScale; }

private:
    int       type;
    bool      enabled;
    glm::vec3 color;
    float     intensity;
    float     ambientScale;
    float     diffuseScale;
    float     specularScale;
    glm::vec3 direction;
    float     constantAtt;
    float     linearAtt;
    float     quadraticAtt;

    GLuint gizmoVAO;
    GLuint gizmoVBO;
    GLuint gizmoEBO;
    bool   gizmoReady;
    int    lineVertexCount;

    void buildGizmo();
};

#endif // LIGHT_H
