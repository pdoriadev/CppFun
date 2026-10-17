#ifndef TORUS_H
#define TORUS_H

#include "Shape.h"

// A torus: a circle of radius r swept around the Y axis at distance R.
//
// THIS IS THE WORKED EXAMPLE for Assignment 2's procedural shapes. It is given,
// complete and commented for reading rather than for brevity, because it has
// the same skeleton the Sphere wants:
//
//   1. two parameters, u and v, each running over a fixed range
//   2. a nested loop that walks a grid over them and pushes one vertex per
//      grid point, in a documented order
//   3. a second loop that stitches each grid CELL into two triangles
//   4. one call to calculateNormals()
//   5. the given buffer upload
//
// A student who understands why step 2 and step 3 are separate loops -- and why
// the vertex order in step 2 has to be pinned down before step 3 can index into
// it -- has the sphere, the cylinder, the cone and their own custom shape.
//
// The torus is chosen over a sphere for this role deliberately: it has no
// degenerate poles and no seam ambiguity, so the loop structure is visible
// without the two special cases the sphere adds on top. Those are left for the
// student to meet on their own.
class Torus : public Shape {
public:
    const char* serialType() const override { return "Torus"; }

    Torus(float x, float y, float z, float scale, int colorIndex, int id);
    ~Torus();

    void draw(GLuint shaderProgram) override;
    void setupTorus();

private:
    GLuint VAO, VBO, EBO;
    int indexCount;
};

#endif // TORUS_H
