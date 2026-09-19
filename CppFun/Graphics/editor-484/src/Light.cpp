#include "Light.h"

#include <glm/gtc/type_ptr.hpp>
#include <cmath>

Light::Light(float x, float y, float z, int id)
    : Shape(x, y, z, 1.0f, 30, id),   // preset 30 is a pale yellow
      type(Point),
      enabled(true),
      color(1.0f, 1.0f, 1.0f),
      intensity(1.0f),
      // Defaults reproduce the fixed light the scene used to have:
      // ambient 0.15, diffuse 1.0, specular 0.8.
      ambientScale(0.15f),
      diffuseScale(1.0f),
      specularScale(0.8f),
      direction(0.0f, -1.0f, 0.0f),
      constantAtt(1.0f),
      linearAtt(0.0f),
      quadraticAtt(0.0f),
      gizmoVAO(0), gizmoVBO(0), gizmoEBO(0), gizmoReady(false), lineVertexCount(0) {

    setShapeType("Light");

    // A small octahedron. This is only ever drawn as a wireframe marker - it is
    // excluded from ray geometry - but populating the shared vertex/face arrays
    // keeps the shape machinery (bounds, selection) working uniformly.
    const float r = 0.25f;
    std::vector<glm::vec3> v;
    v.push_back(glm::vec3( r, 0.0f, 0.0f));
    v.push_back(glm::vec3(-r, 0.0f, 0.0f));
    v.push_back(glm::vec3(0.0f,  r, 0.0f));
    v.push_back(glm::vec3(0.0f, -r, 0.0f));
    v.push_back(glm::vec3(0.0f, 0.0f,  r));
    v.push_back(glm::vec3(0.0f, 0.0f, -r));
    setVertices(v);

    const int tris[8][3] = {
        {0,2,4}, {2,1,4}, {1,3,4}, {3,0,4},
        {2,0,5}, {1,2,5}, {3,1,5}, {0,3,5}
    };
    std::vector<std::vector<int> > f;
    for (int i = 0; i < 8; ++i) {
        std::vector<int> t;
        t.push_back(tris[i][0]); t.push_back(tris[i][1]); t.push_back(tris[i][2]);
        f.push_back(t);
    }
    setFaces(f);
}

Light::~Light() {
    if (gizmoVAO) glDeleteVertexArrays(1, &gizmoVAO);
    if (gizmoVBO) glDeleteBuffers(1, &gizmoVBO);
    if (gizmoEBO) glDeleteBuffers(1, &gizmoEBO);
}

void Light::buildGizmo() {
    // Built fresh each frame from line segments: the marker changes with the
    // light's colour, type, direction and enabled state, and it is a few dozen
    // vertices, so caching would cost more complexity than it saves.
    lineVertexCount = 0;

    // A disabled light stays visible so it can still be selected, just dimmed.
    glm::vec3 c = enabled ? color : color * 0.25f;

    std::vector<float> data;
    // Local lambda: append one line segment with a colour.
    struct Appender {
        std::vector<float>& d;
        void add(const glm::vec3& a, const glm::vec3& b, const glm::vec3& col) {
            d.push_back(a.x); d.push_back(a.y); d.push_back(a.z);
            d.push_back(col.r); d.push_back(col.g); d.push_back(col.b);
            d.push_back(b.x); d.push_back(b.y); d.push_back(b.z);
            d.push_back(col.r); d.push_back(col.g); d.push_back(col.b);
        }
    } out = { data };

    const float r = 0.30f;   // octahedron body
    const float R = 0.75f;   // radiating spikes

    glm::vec3 px( r, 0, 0), nx(-r, 0, 0);
    glm::vec3 py( 0, r, 0), ny( 0,-r, 0);
    glm::vec3 pz( 0, 0, r), nz( 0, 0,-r);

    // Octahedron silhouette - reads as a small solid body.
    out.add(px, py, c); out.add(py, nx, c); out.add(nx, ny, c); out.add(ny, px, c);
    out.add(px, pz, c); out.add(py, pz, c); out.add(nx, pz, c); out.add(ny, pz, c);
    out.add(px, nz, c); out.add(py, nz, c); out.add(nx, nz, c); out.add(ny, nz, c);

    // Radiating spikes, so it reads as a light rather than just a shape.
    out.add(glm::vec3(-R,0,0), glm::vec3(R,0,0), c);
    out.add(glm::vec3(0,-R,0), glm::vec3(0,R,0), c);
    out.add(glm::vec3(0,0,-R), glm::vec3(0,0,R), c);
    // Diagonal spikes, shorter, for a denser star.
    const float d = R * 0.45f;
    out.add(glm::vec3(-d,-d,-d), glm::vec3(d, d, d), c);
    out.add(glm::vec3(-d, d,-d), glm::vec3(d,-d, d), c);
    out.add(glm::vec3( d,-d,-d), glm::vec3(-d, d, d), c);
    out.add(glm::vec3( d, d,-d), glm::vec3(-d,-d, d), c);

    // Directional lights get an arrow showing which way the light travels.
    if (type == Directional) {
        glm::vec3 dir = direction;
        float len = glm::length(dir);
        if (len > 1e-6f) {
            dir /= len;
            glm::vec3 tip = dir * 2.0f;
            out.add(glm::vec3(0.0f), tip, c);

            // Two barbs, built from any vector not parallel to dir.
            glm::vec3 helper = (std::fabs(dir.y) < 0.9f) ? glm::vec3(0,1,0) : glm::vec3(1,0,0);
            glm::vec3 side = glm::normalize(glm::cross(dir, helper)) * 0.25f;
            glm::vec3 back = tip - dir * 0.5f;
            out.add(tip, back + side, c);
            out.add(tip, back - side, c);
        }
    }

    lineVertexCount = static_cast<int>(data.size() / 6);

    if (!gizmoVAO) glGenVertexArrays(1, &gizmoVAO);
    if (!gizmoVBO) glGenBuffers(1, &gizmoVBO);

    glBindVertexArray(gizmoVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float),
                 data.empty() ? 0 : &data[0], GL_DYNAMIC_DRAW);

    // Layout matches the vertex shader: location 0 position, location 2 colour.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    gizmoReady = true;
}

void Light::draw(GLuint shaderProgram) {
    buildGizmo();
    if (lineVertexCount == 0) return;

    glUseProgram(shaderProgram);

    // Unlit: a light source should not be shaded by itself or by other lights.
    // The colour comes from the per-vertex attribute, which is what the
    // shader's useLighting == 0 path reads.
    GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
    if (lightingLoc != -1) glUniform1i(lightingLoc, 0);

    glBindVertexArray(gizmoVAO);
    glDrawArrays(GL_LINES, 0, lineVertexCount);
    glBindVertexArray(0);

    if (lightingLoc != -1) glUniform1i(lightingLoc, 1);
}
