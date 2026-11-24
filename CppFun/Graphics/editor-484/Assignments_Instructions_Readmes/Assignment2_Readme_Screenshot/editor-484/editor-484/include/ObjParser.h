#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <istream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

// =============================================================================
// Wavefront OBJ parsing
// =============================================================================
// Split out of FileImporter.cpp deliberately. That file keeps everything that
// is always given: the file dialog, shape construction, registration with the
// ShapeManager, naming. This holds only the parse, which is the student's work.
//
// Two reasons for the split, and both matter:
//
//   testable    it takes an istream, not a path, so a test can parse a string
//               with no file on disk, no dialog and no GL context. See
//               tests/obj_parser_test.cpp.
//   swappable   the unit a solution library can replace is a whole translation
//               unit, so the student's code has to live in a file of its own.
//               See SOLUTION-LIBRARIES.md and ASSIGNMENTS.md.

struct ObjMesh {
    std::vector<glm::vec3>        vertices;   // v
    std::vector<glm::vec3>        normals;    // vn
    std::vector<std::vector<int> > faces;     // per corner: vertex index, normal index

    // Texture coordinates, resolved from vt at parse time and stored one per
    // TRIANGLE CORNER rather than one per vertex.
    //
    // Storing them per vertex does not work: in an OBJ a single v can be
    // referenced with a different vt by each face that uses it -- that is the
    // whole point of the v/vt/vn triple. Resolving here also means the face
    // index stride stays 2 (vertex, normal) rather than widening to 3, so
    // nothing downstream has to learn a new layout. RayScene recognises this
    // per-corner layout by size; Cube uses the same one.
    std::vector<glm::vec2>        texCoords;
    bool                          hasTexCoords;

    ObjMesh() : hasTexCoords(false) {}
};

// Parses an OBJ stream. Returns false only when nothing usable was found;
// malformed individual lines are skipped rather than aborting the parse, since
// a mesh with one bad face is still worth showing.
//
// Handles:
//   v  x y z
//   vn x y z
//   vt u v [w]        -- w ignored
//   f  a/b/c d/e/f g/h/i        with b, e, h optional ("a//c")
//
// Triangles only: a face with more than three corners keeps its first three.
// Fan triangulation happens later, in RayScene, where the geometry is
// flattened anyway.
bool parseObj(std::istream& in, ObjMesh& out);

#endif // OBJPARSER_H
