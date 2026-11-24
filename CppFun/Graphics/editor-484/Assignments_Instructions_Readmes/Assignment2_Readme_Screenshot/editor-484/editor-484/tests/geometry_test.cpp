// Grader for the `geometry` topic: shape generation and face normals.
//
// A2, both courses, alongside transform_test.
//
// PROPERTY-BASED ON PURPOSE. There is no expected vertex list anywhere in this
// file. A student may tessellate a sphere with 20 segments or 60, wind their
// cube's faces starting from any corner, and invent whatever custom shape they
// like -- all of those are correct, and a grader built on golden data would
// reject most of them. So this checks the things that are true of any correct
// mesh and false of a broken one:
//
//   * every face index is in range
//   * normals are unit length and there is one per face or one per vertex
//   * the shape is CLOSED, and wound consistently
//   * the extent matches the one-unit convention the whole editor assumes
//
// The closedness and winding checks lean on the divergence theorem rather than
// on counting shared edges, because counting edges by vertex index breaks the
// moment a generator duplicates its seam or pole vertices -- which the sphere
// legitimately does. Two consequences of a closed, outward-wound surface:
//
//   sum of area-weighted face normals  ==  0
//   signed volume  =  (1/6) * sum of v0 . (v1 x v2)  is far from zero
//
// Both hold whatever the tessellation, and both fail loudly for the two
// mistakes students actually make: a hole in the mesh, and a face wound
// backwards.

#include "gl_context.h"

#include "Shape.h"
#include "Cube.h"
#include "Pyramid.h"
#include "Sphere.h"
#include "Icosahedron.h"
#include "Custom.h"
#include "Torus.h"

#include <cstdio>
#include <cmath>
#include <string>
#include <vector>

static int checks = 0, failures = 0;

static void ok(bool cond, const std::string& what) {
    ++checks;
    if (cond) std::printf("  PASS  %s\n", what.c_str());
    else    { std::printf("  FAIL  %s\n", what.c_str()); ++failures; }
}

// --- the two integrals -------------------------------------------------------

static glm::vec3 areaWeightedNormalSum(const Shape& s) {
    glm::vec3 total(0.0f);
    const std::vector<glm::vec3>& v = s.getVertices();
    const std::vector<std::vector<int> >& f = s.getFaces();

    for (size_t i = 0; i < f.size(); ++i) {
        if (f[i].size() < 3) continue;
        // The cross product's LENGTH is twice the triangle's area, so leaving
        // it unnormalised is what makes this an area-weighted sum -- and the
        // area weighting is what makes it cancel exactly on a closed surface.
        total += glm::cross(v[f[i][1]] - v[f[i][0]], v[f[i][2]] - v[f[i][0]]);
    }
    return total;
}

static float signedVolume(const Shape& s) {
    float vol = 0.0f;
    const std::vector<glm::vec3>& v = s.getVertices();
    const std::vector<std::vector<int> >& f = s.getFaces();

    for (size_t i = 0; i < f.size(); ++i) {
        if (f[i].size() < 3) continue;
        vol += glm::dot(v[f[i][0]], glm::cross(v[f[i][1]], v[f[i][2]]));
    }
    return vol / 6.0f;
}

// --- per-shape checks --------------------------------------------------------

// `closed` is false for shapes whose outline is the student's choice -- their
// custom shape may legitimately be a plane or a strip.
static void checkShape(const Shape& s, const std::string& name,
                       bool closed, size_t minFaces) {
    const std::vector<glm::vec3>& verts = s.getVertices();
    const std::vector<glm::vec3>& norms = s.getNormals();
    const std::vector<std::vector<int> >& faces = s.getFaces();

    ok(!verts.empty() && !faces.empty(), name + ": produces geometry at all");
    if (verts.empty() || faces.empty()) return;

    ok(faces.size() >= minFaces,
       name + ": has at least the required number of faces");

    // Indices in range. An out-of-range index reads memory that is not ours --
    // it may look fine in the viewport today and abort inside the ray tracer in
    // Assignment 6.
    bool inRange = true, triangles = true;
    for (size_t i = 0; i < faces.size(); ++i) {
        if (faces[i].size() != 3) triangles = false;
        for (size_t k = 0; k < faces[i].size(); ++k) {
            if (faces[i][k] < 0 || faces[i][k] >= static_cast<int>(verts.size())) {
                inRange = false;
            }
        }
    }
    ok(triangles, name + ": every face is a triangle");
    ok(inRange, name + ": every face index is a valid index into vertices");
    if (!inRange) return;   // nothing below is meaningful with bad indices




    // Three conventions are accepted, because three exist in this codebase:
    //   per face      normals.size() == faces.size()      Pyramid, Icosahedron
    //   per vertex    normals.size() == vertices.size()   Sphere, Custom
    //   per face-pair normals.size() == faces.size() / 2  Cube, whose twelve
    //                 triangles form six flat quads
    //
    // Only the first two are understood downstream: RayScene decides whether to
    // trust supplied normals by comparing against vertices.size(), so the
    // cube's six fall through to geometric face normals in the ray tracer. That
    // happens to look identical for a cube -- its faces are flat -- but it
    // means those six normals are used by nothing except the cube's own draw
    // loop. See ASSIGNMENTS.md, A2, open questions.
    const bool perFace   = norms.size() == faces.size();
    const bool perVertex = norms.size() == verts.size();
    const bool perPair   = faces.size() % 2 == 0 && norms.size() == faces.size() / 2;
    ok(perFace || perVertex || perPair,
       name + ": normals.size() matches faces, vertices, or face-pairs -- the "
              "renderer decides flat vs smooth shading from that alone");

    bool unit = true;
    for (size_t i = 0; i < norms.size(); ++i) {
        const float len = glm::length(norms[i]);
        if (len < 0.99f || len > 1.01f) unit = false;
        if (norms[i].x != norms[i].x) unit = false;   // NaN, from normalising zero
    }
    ok(unit, name + ": every normal is unit length and finite");

    // Extent. Everything is built roughly one unit across and centred, so that
    // scale is the model matrix's job and scenes are comparable between
    // students.
    glm::vec3 lo = verts[0], hi = verts[0];
    for (size_t i = 1; i < verts.size(); ++i) {
        lo = glm::min(lo, verts[i]);
        hi = glm::max(hi, verts[i]);
    }
    const glm::vec3 size = hi - lo;
    const float biggest = glm::max(size.x, glm::max(size.y, size.z));
    // Upper bound is 2.2 rather than ~1.2 because the given superellipsoid
    // spans -1..1 while every other shape spans -0.5..0.5. Tightening this is
    // an open question -- see ASSIGNMENTS.md, A2.
    ok(biggest > 0.3f && biggest < 2.2f,
       name + ": spans roughly one unit before the model matrix");

    const glm::vec3 centre = (lo + hi) * 0.5f;
    ok(glm::length(centre) < 0.35f,
       name + ": is centred on its own origin, so it rotates about itself");

    if (!closed) return;

    // Closed.
    const glm::vec3 nsum = areaWeightedNormalSum(s);
    const float scale = biggest * biggest;      // the sum has units of area
    ok(glm::length(nsum) < 0.02f * scale,
       name + ": the area-weighted face normals cancel -- the surface is "
              "closed, with no missing or duplicated faces");

    // CONSISTENCY, not direction. A mesh whose faces are all wound the same way
    // encloses a definite volume; one with some faces reversed has parts of its
    // surface cancelling against the rest, and the total collapses towards zero.
    // That is the mistake worth catching, and it is the one students make.
    //
    // The SIGN is a separate matter and is deliberately not asserted: the
    // shapes shipped with this editor disagree about it (the cube is wound one
    // way, the sphere, torus and superellipsoid the other), and nothing here
    // notices, because both renderers orient the shading normal against the
    // view direction before using it. It does matter for refraction sidedness
    // in Assignment 6. See ASSIGNMENTS.md, A2, open questions.
    const float vol = signedVolume(s);
    const float cube_ = biggest * biggest * biggest;
    ok(std::fabs(vol) > 0.02f * cube_,
       name + ": the signed volume is substantial -- every face is wound the "
              "same way, so the surface encloses a definite volume instead of "
              "cancelling against itself");
}

int main() {
    std::printf("Shape geometry\n");

    if (!makeHeadlessContext()) {
        std::printf("no GL context -- skipped\n");
        return 0;      // shape constructors create VAOs
    }

    // --- calculateNormals, on its own ---------------------------------------
    //
    // Checked directly before any shape is built, because every failure below
    // would otherwise be blamed on the generator.
    {
        class Tri : public Shape {
        public:
            Tri() : Shape(0.0f, 0.0f, 0.0f, 1.0f, 0, 1) { shapeType = "Tri"; }
            void draw(GLuint) override {}
            void build() {
                vertices.clear(); faces.clear();
                vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
                vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
                vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
                std::vector<int> f;
                f.push_back(0); f.push_back(1); f.push_back(2);
                faces.push_back(f);
                calculateNormals();
            }
            void buildDegenerate() {
                vertices.clear(); faces.clear();
                // Two corners at the same point -- what happens at the poles of
                // a sphere. The cross product is zero and normalising it gives
                // NaN, which becomes a NaN colour and then a speckle nobody can
                // trace back to here.
                vertices.push_back(glm::vec3(0.0f));
                vertices.push_back(glm::vec3(0.0f));
                vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
                std::vector<int> f;
                f.push_back(0); f.push_back(1); f.push_back(2);
                faces.push_back(f);
                calculateNormals();
            }
        };

        Tri t;
        t.build();
        ok(t.getNormals().size() == 1, "calculateNormals: one normal per face");
        if (t.getNormals().size() == 1) {
            const glm::vec3 n = t.getNormals()[0];
            ok(std::fabs(glm::length(n) - 1.0f) < 1e-4f,
               "calculateNormals: the normal is a UNIT vector -- an unnormalised "
               "one silently scales every lighting term that uses it");
            ok(n.z > 0.9f,
               "calculateNormals: (b-a) x (c-a) and the right-hand rule -- a "
               "counter-clockwise triangle in the XY plane faces +Z");
        } else {
            checks += 2; failures += 2;
        }

        Tri d;
        d.buildDegenerate();
        bool finite = true;
        for (size_t i = 0; i < d.getNormals().size(); ++i) {
            const glm::vec3& n = d.getNormals()[i];
            if (n.x != n.x || n.y != n.y || n.z != n.z) finite = false;
        }
        ok(finite,
           "calculateNormals: a DEGENERATE triangle gives a finite normal, not "
           "NaN -- this is the sphere's poles, and it will happen to you");
    }

    // --- the shapes ---------------------------------------------------------
    {
        Cube        cube(0, 0, 0, 1.0f, 3, 1);
        Pyramid     pyr (0, 0, 0, 1.0f, 3, 2);
        Sphere      sph (0, 0, 0, 1.0f, 3, 3);
        Icosahedron ico (0, 0, 0, 1.0f, 3, 4);
        Custom      cus (0, 0, 0, 1.0f, 3, 5);

        checkShape(cube, "Cube",        true,  12);
        checkShape(pyr,  "Pyramid",     true,   6);
        checkShape(sph,  "Sphere",      true,  50);
        checkShape(ico,  "Icosahedron", true,  20);

        // The custom shape is the student's design, so only the contract is
        // checked -- not that it is closed, and not what it looks like. The
        // twelve-face minimum is the assignment's, and it is there to rule out
        // a single quad passing as a shape.
        checkShape(cus, "Custom shape", false, 12);
    }

    // --- the given worked example -------------------------------------------
    //
    // Torus is given, so this is a tripwire rather than a grade: if it ever
    // fails, the conventions this file enforces have drifted away from the code
    // the handout points students at as the model answer.
    {
        Torus torus(0, 0, 0, 1.0f, 3, 6);
        checkShape(torus, "Torus (given)", true, 100);
    }

    std::printf("\n%d checks, %d failures\n", checks, failures);
    if (failures == 0) { std::printf("ALL TESTS PASSED (0 failures)\n"); return 0; }
    std::printf("TESTS FAILED (%d failures)\n", failures);
    return 1;
}
