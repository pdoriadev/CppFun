// Wavefront OBJ parsing -- assertions for CPSC 484 Assignment 3.
//
// Fully headless and file-free: parseObj takes an istream, so every case here
// is a string literal. That is exactly why the parse was split out of
// FileImporter, which needs a dialog, a ShapeManager and a GL context before
// it will parse a single line.

#include <cstdio>
#include <cmath>
#include <sstream>
#include <string>

#include "ObjParser.h"

static int failures = 0;
static int checks   = 0;

static void ok(bool cond, const std::string& what) {
    ++checks;
    if (!cond) { ++failures; std::printf("  FAIL  %s\n", what.c_str()); }
}

static void nearly(float a, float b, const std::string& what, float eps = 1e-5f) {
    ++checks;
    if (std::fabs(a - b) > eps) {
        ++failures;
        std::printf("  FAIL  %s   (got %.6f, expected %.6f)\n",
                    what.c_str(), a, b);
    }
}

static bool parse(const std::string& text, ObjMesh& out) {
    std::istringstream in(text);
    return parseObj(in, out);
}

int main() {
    std::printf("Wavefront OBJ parsing\n");

    // --- the minimal valid file ---------------------------------------------
    {
        ObjMesh m;
        ok(parse("v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\nf 1//1 2//1 3//1\n", m),
           "a triangle with v and vn parses");
        ok(m.vertices.size() == 3, "three vertices");
        ok(m.normals.size() == 1,  "one normal");
        ok(m.faces.size() == 1,    "one face");
        ok(!m.hasTexCoords,        "no vt lines means no texture coordinates");
        ok(m.texCoords.empty(),
           "and the coordinate list is left empty, not filled with zeros");

        // Faces carry (vertex, normal) per corner: stride 2, not 3. The vt
        // index is resolved at parse time instead of being stored.
        //
        // Guarded, like every other indexing check here: against an unfinished
        // stub `faces` is empty, and a grader that segfaults tells the student
        // nothing. Every assertion must survive the empty case and report.
        ok(!m.faces.empty() && m.faces[0].size() == 6,
           "a triangle stores six ints: three v/n pairs");
        if (!m.faces.empty() && m.faces[0].size() == 6) {
            ok(m.faces[0][0] == 0 && m.faces[0][2] == 1 && m.faces[0][4] == 2,
               "vertex indices are converted from 1-based to 0-based");
            ok(m.faces[0][1] == 0 && m.faces[0][3] == 0 && m.faces[0][5] == 0,
               "normal indices are 0-based too");
        }
    }

    // --- coordinates come through unmangled ---------------------------------
    {
        ObjMesh m;
        parse("v -1.5 2.25 -0.125\nv 1 0 0\nv 0 1 0\nvn 0 1 0\n"
              "f 1//1 2//1 3//1\n", m);
        ok(m.vertices.size() == 3, "three vertices");
        if (m.vertices.size() == 3) {
            nearly(m.vertices[0].x, -1.5f,   "negative x");
            nearly(m.vertices[0].y,  2.25f,  "fractional y");
            nearly(m.vertices[0].z, -0.125f, "fractional negative z");
        }
    }

    // --- all four corner forms ----------------------------------------------
    {
        ObjMesh m;
        // "v", "v/vt", "v//vn" and "v/vt/vn" are all legal OBJ.
        ok(parse("v 0 0 0\nv 1 0 0\nv 0 1 0\n"
                 "vt 0 0\nvt 1 0\nvt 0 1\n"
                 "vn 0 0 1\n"
                 "f 1/1/1 2/2 3//1\n", m),
           "mixed corner forms in one face parse");
        ok(m.faces.size() == 1, "still one face");
        ok(m.hasTexCoords, "the corners that had a vt contribute one");
    }
    {
        ObjMesh m;
        ok(parse("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n", m),
           "a face with bare vertex indices parses");
        ok(m.faces.size() == 1, "one face");
        if (!m.faces.empty() && m.faces[0].size() == 6) {
            // With no vn, the normal index falls back to the vertex index --
            // which is what a stride-2 face means elsewhere in the project, and
            // RayScene drops to geometric normals when they do not line up.
            ok(m.faces[0][1] == 0 && m.faces[0][3] == 1 && m.faces[0][5] == 2,
               "a missing normal index falls back to the vertex index");
        }
    }

    // --- texture coordinates ------------------------------------------------
    {
        ObjMesh m;
        parse("v 0 0 0\nv 1 0 0\nv 1 1 0\nv 0 1 0\n"
              "vt 0 0\nvt 1 0\nvt 1 1\nvt 0 1\n"
              "vn 0 0 1\n"
              "f 1/1/1 2/2/1 3/3/1\n"
              "f 1/1/1 3/3/1 4/4/1\n", m);

        ok(m.hasTexCoords, "vt lines are recognised");
        // Per CORNER, not per vertex: in an OBJ the same v can be paired with a
        // different vt by every face that uses it. That is the whole reason the
        // v/vt/vn triple exists.
        ok(m.texCoords.size() == m.faces.size() * 3,
           "texture coordinates are stored one per triangle corner");
        ok(m.texCoords.size() != m.vertices.size(),
           "and deliberately NOT one per vertex");

        if (m.texCoords.size() == 6) {
            nearly(m.texCoords[0].x, 0.0f, "corner 0 u");
            nearly(m.texCoords[1].x, 1.0f, "corner 1 u");
            nearly(m.texCoords[2].y, 1.0f, "corner 2 v");
            nearly(m.texCoords[5].x, 0.0f, "second face, third corner u");
            nearly(m.texCoords[5].y, 1.0f, "second face, third corner v");
        }
    }
    {
        // A vt with the optional third (w) component must not shift the parse.
        ObjMesh m;
        parse("v 0 0 0\nv 1 0 0\nv 0 1 0\n"
              "vt 0.25 0.75 0\nvt 1 0 0\nvt 0 1 0\n"
              "vn 0 0 1\nf 1/1/1 2/2/1 3/3/1\n", m);
        if (m.texCoords.size() >= 1) {
            nearly(m.texCoords[0].x, 0.25f, "vt with a w component: u survives");
            nearly(m.texCoords[0].y, 0.75f, "vt with a w component: v survives");
        }
    }

    // --- negative indices ---------------------------------------------------
    //
    // OBJ allows an index to count backwards from the end of the list so far.
    // Exporters do emit these, and a parser that ignores the sign silently
    // produces garbage geometry rather than an error.
    {
        ObjMesh m;
        ok(parse("v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\nf -3//-1 -2//-1 -1//-1\n", m),
           "negative indices parse");
        if (!m.faces.empty() && m.faces[0].size() == 6) {
            ok(m.faces[0][0] == 0 && m.faces[0][2] == 1 && m.faces[0][4] == 2,
               "-3, -2, -1 resolve to the last three vertices, in order");
        }
    }

    // --- things that must be ignored, not choke -----------------------------
    {
        ObjMesh m;
        ok(parse("# a comment\n"
                 "mtllib scene.mtl\n"
                 "o Cube\n"
                 "g group1\n"
                 "s off\n"
                 "usemtl Material\n"
                 "\n"
                 "v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\n"
                 "f 1//1 2//1 3//1\n", m),
           "comments, blank lines and unsupported directives are skipped");
        ok(m.vertices.size() == 3 && m.faces.size() == 1,
           "and the real content still comes through");
    }
    {
        // Windows line endings. A trailing CR glued to the last field turns
        // "1//1\r" into an unparseable index if it is not stripped.
        ObjMesh m;
        ok(parse("v 0 0 0\r\nv 1 0 0\r\nv 0 1 0\r\nvn 0 0 1\r\nf 1//1 2//1 3//1\r\n", m),
           "a file with CRLF line endings parses");
        ok(m.faces.size() == 1, "the face survived the carriage returns");
    }

    // --- malformed input ----------------------------------------------------
    {
        ObjMesh m;
        ok(!parse("", m), "an empty stream reports failure");
        ok(!parse("v 0 0 0\nv 1 0 0\n", m), "vertices with no faces report failure");
        ok(!parse("f 1//1 2//1 3//1\n", m), "faces with no vertices report failure");
    }
    {
        // One bad face must not abandon the rest of the file.
        ObjMesh m;
        ok(parse("v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\n"
                 "f 1//1 99//1 3//1\n"          // out of range
                 "f 1//1 2//1\n"                 // only two corners
                 "f 1//1 2//1 3//1\n", m),       // fine
           "a file with some bad faces still parses");
        ok(m.faces.size() == 1, "only the well-formed face is kept");
    }

    std::printf("\n%d checks, %d failures\n", checks, failures);
    if (failures == 0) { std::printf("ALL TESTS PASSED (0 failures)\n"); return 0; }
    std::printf("TESTS FAILED (%d failures)\n", failures);
    return 1;
}
