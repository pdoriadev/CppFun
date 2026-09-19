// =============================================================================
// Shape::calculateNormals -- face normals from geometry
// =============================================================================
// One function, in a file of its own. It is still a member of Shape; only the
// definition lives here. Shape.h declares it, nothing about the class changed,
// and every caller is unaffected -- the linker simply finds the symbol in
// ShapeNormals.o instead of Shape.o.
//
// The point of the split is that a translation unit is the smallest thing a
// solution archive can swap. Shape.cpp is 260 lines of given code -- the
// constructor, the accessors, the material and buffer plumbing -- and none of
// that is the student's work, so it cannot be swapped as a unit. This can.
// Same surgery BVH.cpp needed for the `bvh` topic and RayTracer.cpp for
// `raytracer`. See SOLUTION-LIBRARIES.md and ASSIGNMENTS.md.
//
// A2: Geometry and Transformations, in both courses.

#include "Shape.h"

#include <cmath>

void Shape::calculateNormals() {
    // TODO(geometry): one normal per face, from the cross product of two edges
    // Fill `normals` with ONE unit normal per entry in `faces`, in the same
    // order, computed from the geometry in `vertices`.
    //
    // For a triangle with corners a, b, c:
    //     v = (b - a) x (c - a)
    //     n = v / |v|
    //
    // The cross product is not commutative: swapping the two edges flips the
    // normal. Which one you get depends on the order the corners are listed
    // in -- the winding order -- so a face wound the wrong way lights up from
    // the inside while its neighbours light up from the outside.
    //
    // Two things will bite you if you skip them:
    //   - a DEGENERATE triangle (two corners at the same point, which is what
    //     happens at the poles of a sphere) has a zero-length cross product,
    //     and normalising zero gives NaN. A NaN normal becomes a NaN colour
    //     and then a speckle you will not be able to trace back to here.
    //   - a face index outside `vertices` reads memory that is not yours.
    //     While your shape generators are half-written, that WILL happen.
    //
    // Leaving it as-is gives every face a normal pointing straight up: the
    // scene renders, lit as though every surface were the ground. That is a
    // usable checkpoint, and fixing this one function lights up the Teapot,
    // the Torus and the Mobius strip all at once.
    normals.assign(faces.size(), glm::vec3(0.0f, 1.0f, 0.0f));
}
