#include "RayScene.h"

#include "Shape.h"
#include "ShapeManager.h"
#include "Light.h"

#include <glm/gtc/matrix_inverse.hpp>

RayScene::RayScene() : shapesIncluded(0) {}

void RayScene::clear() {
    triangles.clear();
    lights.clear();
    bvh.clear();
    shapesIncluded = 0;
}

void RayScene::build(ShapeManager& shapeManager) {
    clear();

    std::vector<Shape*>& shapes = shapeManager.getShapes();

    for (size_t s = 0; s < shapes.size(); ++s) {
        Shape* shape = shapes[s];
        if (!shape) continue;

        const std::vector<glm::vec3>& verts   = shape->getVertices();
        const std::vector<std::vector<int> >& faces = shape->getFaces();
        const std::vector<glm::vec3>& normals = shape->getNormals();

        // Lights are scene objects but not geometry: they must never occlude
        // anything or appear as a surface in the render.
        if (!shape->providesRayGeometry()) continue;

        if (verts.empty() || faces.empty()) continue;   // nothing tracable here

        glm::mat4 model = shape->getModelMatrix();
        // Normals need the inverse-transpose whenever scaling is non-uniform,
        // otherwise they shear away from the surface and shading goes wrong.
        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));

        // Some shapes (ImportShape) interleave normal indices with vertex
        // indices inside each face entry; ask the shape rather than guessing.
        const int stride = shape->getFaceIndexStride();

        // With stride 1 we can only trust `normals` if it is indexed the same
        // way as `vertices`. Cube, for instance, stores 6 per-face normals for
        // 8 vertices, so indexing it by vertex would be wrong - fall back to
        // geometric face normals there. With stride 2 the face carries explicit
        // normal indices, so per-vertex normals are always usable.
        const bool haveNormals = (stride == 2) || (normals.size() == verts.size());

        // Texture coordinates arrive in one of two shapes, and which one is
        // decidable from the size alone -- the same trick haveNormals uses
        // above, and for the same reason.
        //
        //   per vertex   texCoords parallels vertices, so a UV is looked up by
        //                vertex index. Sphere: its parameterisation IS its UV
        //                map, one per unique vertex.
        //   per corner   texCoords parallels the emitted triangle corners, so
        //                a UV is looked up by face*3 + corner. Cube: its eight
        //                shared corners each belong to three faces wanting
        //                three different UVs, so a per-vertex map cannot exist.
        //
        // Getting this wrong is the classic cube-texturing trap; deciding it
        // by size means the tracer and the viewport cannot disagree.
        const std::vector<glm::vec2>& uvs = shape->getTexCoords();
        const bool uvPerVertex = !uvs.empty() && uvs.size() == verts.size();
        const bool uvPerCorner = !uvs.empty() && uvs.size() == faces.size() * 3;

        size_t before = triangles.size();

        for (size_t f = 0; f < faces.size(); ++f) {
            const std::vector<int>& face = faces[f];
            const size_t cornerCount = face.size() / static_cast<size_t>(stride);
            if (cornerCount < 3) continue;

            // Fan-triangulate: faces are usually triangles, but importers can
            // produce quads or larger polygons.
            for (size_t k = 1; k + 1 < cornerCount; ++k) {
                int i0 = face[0];
                int i1 = face[k * stride];
                int i2 = face[(k + 1) * stride];

                int n0 = (stride == 2) ? face[1]                  : i0;
                int n1 = (stride == 2) ? face[k * stride + 1]     : i1;
                int n2 = (stride == 2) ? face[(k + 1) * stride + 1] : i2;

                if (i0 < 0 || i1 < 0 || i2 < 0) continue;
                if (static_cast<size_t>(i0) >= verts.size() ||
                    static_cast<size_t>(i1) >= verts.size() ||
                    static_cast<size_t>(i2) >= verts.size()) continue;

                RayTriangle tri;
                tri.v0 = glm::vec3(model * glm::vec4(verts[i0], 1.0f));
                tri.v1 = glm::vec3(model * glm::vec4(verts[i1], 1.0f));
                tri.v2 = glm::vec3(model * glm::vec4(verts[i2], 1.0f));

                // Drop degenerate triangles - they contribute nothing and would
                // produce a zero-length normal.
                glm::vec3 cr = glm::cross(tri.v1 - tri.v0, tri.v2 - tri.v0);
                if (glm::length(cr) < 1e-14f) continue;

                if (haveNormals &&
                    n0 >= 0 && n1 >= 0 && n2 >= 0 &&
                    static_cast<size_t>(n0) < normals.size() &&
                    static_cast<size_t>(n1) < normals.size() &&
                    static_cast<size_t>(n2) < normals.size()) {
                    tri.n0 = normalMatrix * normals[n0];
                    tri.n1 = normalMatrix * normals[n1];
                    tri.n2 = normalMatrix * normals[n2];
                    tri.hasVertexNormals = true;
                }

                if (uvPerVertex) {
                    tri.t0 = uvs[i0];
                    tri.t1 = uvs[i1];
                    tri.t2 = uvs[i2];
                    tri.hasTexCoords = true;
                } else if (uvPerCorner && cornerCount == 3) {
                    // Only for genuine triangles: a fan-triangulated quad has
                    // more emitted corners than this indexing assumes.
                    const size_t base = f * 3;
                    tri.t0 = uvs[base + 0];
                    tri.t1 = uvs[base + 1];
                    tri.t2 = uvs[base + 2];
                    tri.hasTexCoords = true;
                }

                tri.shape = shape;
                triangles.push_back(tri);
            }
        }

        if (triangles.size() > before) ++shapesIncluded;
    }

    // Snapshot the lights alongside the geometry.
    std::vector<Light*> sceneLights = shapeManager.getLights();
    for (size_t i = 0; i < sceneLights.size(); ++i) {
        Light* L = sceneLights[i];
        if (!L || !L->isEnabled()) continue;

        SceneLightSample sample;
        sample.type      = L->getType();
        sample.position  = glm::vec3(L->getX(), L->getY(), L->getZ());
        sample.direction = L->getDirection();
        sample.ambient   = L->effectiveAmbient();
        sample.diffuse   = L->effectiveDiffuse();
        sample.specular  = L->effectiveSpecular();
        sample.constant  = L->getConstantAttenuation();
        sample.linear    = L->getLinearAttenuation();
        sample.quadratic = L->getQuadraticAttenuation();
        lights.push_back(sample);
    }

    bvh.build(triangles);
}

bool RayScene::intersect(const Ray& ray, float tMin, float tMax, Hit& out) const {
    if (triangles.empty()) return false;
    return bvh.intersect(triangles, ray, tMin, tMax, out);
}

bool RayScene::occluded(const Ray& ray, float tMin, float tMax, const Shape* ignore) const {
    if (triangles.empty()) return false;
    return bvh.occluded(triangles, ray, tMin, tMax, ignore);
}

float RayScene::transmittance(const Ray& ray, float tMin, float tMax,
                              const Shape* ignore) const {
    if (triangles.empty()) return 1.0f;
    return bvh.transmittance(triangles, ray, tMin, tMax, ignore);
}
