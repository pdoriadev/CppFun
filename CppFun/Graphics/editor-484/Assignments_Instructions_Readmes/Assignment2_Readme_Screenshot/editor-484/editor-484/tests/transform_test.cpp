// Model matrix composition -- assertions for CPSC 484 Assignment 2.
//
// Headless and GL-free: getModelMatrix() is pure maths over the shape's
// position, rotation and scale, so nothing here needs a context. Shape is
// abstract, so the fixture below is a minimal concrete subclass rather than a
// Cube (whose constructor would create VAOs).
//
// This suite is the objective part of the assignment's grade.

#include <cstdio>
#include <cmath>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shape.h"

static int failures = 0;
static int checks   = 0;

static void nearlyVec(const glm::vec3& a, const glm::vec3& b,
                      const std::string& what, float eps = 1e-4f) {
    ++checks;
    if (std::fabs(a.x - b.x) > eps || std::fabs(a.y - b.y) > eps ||
        std::fabs(a.z - b.z) > eps) {
        ++failures;
        std::printf("  FAIL  %s\n"
                    "         got      (%.5f, %.5f, %.5f)\n"
                    "         expected (%.5f, %.5f, %.5f)\n",
                    what.c_str(), a.x, a.y, a.z, b.x, b.y, b.z);
    }
}

// The smallest thing that is a Shape. draw() is never called.
class TestShape : public Shape {
public:
    TestShape() : Shape(0.0f, 0.0f, 0.0f, 1.0f, 0, 1) { shapeType = "Test"; }
    void draw(GLuint) override {}
};

// Transform a point by the shape's model matrix.
static glm::vec3 apply(const Shape& s, const glm::vec3& p) {
    return glm::vec3(s.getModelMatrix() * glm::vec4(p, 1.0f));
}

int main() {
    std::printf("Model matrix composition\n");

    // --- identity -----------------------------------------------------------
    {
        TestShape s;
        nearlyVec(apply(s, glm::vec3(1, 2, 3)), glm::vec3(1, 2, 3),
                  "a default shape leaves points where they are");
    }

    // --- translation --------------------------------------------------------
    {
        TestShape s;
        s.setPosition(4.0f, -2.0f, 0.5f);
        nearlyVec(apply(s, glm::vec3(0, 0, 0)), glm::vec3(4, -2, 0.5f),
                  "the origin lands at the position");
        nearlyVec(apply(s, glm::vec3(1, 1, 1)), glm::vec3(5, -1, 1.5f),
                  "translation is applied to every point equally");
    }

    // --- uniform scale ------------------------------------------------------
    {
        TestShape s;
        s.setScale(3.0f);
        nearlyVec(apply(s, glm::vec3(1, 2, -1)), glm::vec3(3, 6, -3),
                  "uniform scale multiplies every axis");
        nearlyVec(apply(s, glm::vec3(0, 0, 0)), glm::vec3(0, 0, 0),
                  "scaling leaves the origin fixed");
    }

    // --- non-uniform scale --------------------------------------------------
    {
        TestShape s;
        s.useUniformScaling(false);
        s.setScale(2.0f, 0.5f, 4.0f);
        nearlyVec(apply(s, glm::vec3(1, 1, 1)), glm::vec3(2, 0.5f, 4),
                  "non-uniform scale uses one factor per axis");

        // And the uniform flag must actually select between them.
        s.useUniformScaling(true);
        s.setScale(3.0f);
        nearlyVec(apply(s, glm::vec3(1, 1, 1)), glm::vec3(3, 3, 3),
                  "switching back to uniform uses the uniform factor");
    }

    // --- rotation about each axis, right-handed -----------------------------
    {
        TestShape s;
        s.setRotation(0.0f, 90.0f, 0.0f);
        // +90 about Y takes +X to -Z in a right-handed system.
        nearlyVec(apply(s, glm::vec3(1, 0, 0)), glm::vec3(0, 0, -1),
                  "+90 about Y takes +X to -Z");
        nearlyVec(apply(s, glm::vec3(0, 1, 0)), glm::vec3(0, 1, 0),
                  "the Y axis is fixed by a Y rotation");
    }
    {
        TestShape s;
        s.setRotation(90.0f, 0.0f, 0.0f);
        nearlyVec(apply(s, glm::vec3(0, 1, 0)), glm::vec3(0, 0, 1),
                  "+90 about X takes +Y to +Z");
    }
    {
        TestShape s;
        s.setRotation(0.0f, 0.0f, 90.0f);
        nearlyVec(apply(s, glm::vec3(1, 0, 0)), glm::vec3(0, 1, 0),
                  "+90 about Z takes +X to +Y");
    }

    // --- ORDER: this is the point of the assignment -------------------------
    //
    // T * Rz * Ry * Rx * S, read right to left. Scale first in the object's own
    // frame, then rotate, then translate. Any other order is visibly wrong:
    // scaling after rotating shears, and translating before rotating swings the
    // object round the world origin instead of spinning it in place.
    {
        TestShape s;
        s.setPosition(10.0f, 0.0f, 0.0f);
        s.setRotation(0.0f, 90.0f, 0.0f);

        // If translation were applied before the rotation, the shape's own
        // origin would be swung to (0, 0, -10) instead of staying at (10, 0, 0).
        nearlyVec(apply(s, glm::vec3(0, 0, 0)), glm::vec3(10, 0, 0),
                  "rotation happens about the object's own origin, not the world's");
    }
    {
        TestShape s;
        s.useUniformScaling(false);
        s.setScale(2.0f, 1.0f, 1.0f);
        s.setRotation(0.0f, 90.0f, 0.0f);

        // Scale-then-rotate: the x-stretch travels with the object, so a point
        // one unit along local +X ends up two units along world -Z.
        // Rotate-then-scale would stretch along WORLD x instead and give
        // (0, 0, -1).
        nearlyVec(apply(s, glm::vec3(1, 0, 0)), glm::vec3(0, 0, -2),
                  "scale is applied in the object's frame, before the rotation");
    }
    {
        // All three at once, checked against an independently composed matrix.
        TestShape s;
        s.setPosition(1.0f, 2.0f, 3.0f);
        s.setRotation(30.0f, 45.0f, 60.0f);
        s.useUniformScaling(false);
        s.setScale(1.5f, 2.0f, 0.5f);

        glm::mat4 expected(1.0f);
        expected = glm::translate(expected, glm::vec3(1.0f, 2.0f, 3.0f));
        expected = glm::rotate(expected, glm::radians(30.0f), glm::vec3(1, 0, 0));
        expected = glm::rotate(expected, glm::radians(45.0f), glm::vec3(0, 1, 0));
        expected = glm::rotate(expected, glm::radians(60.0f), glm::vec3(0, 0, 1));
        expected = glm::scale(expected, glm::vec3(1.5f, 2.0f, 0.5f));

        const glm::vec3 probes[4] = {
            glm::vec3(0, 0, 0), glm::vec3(1, 0, 0),
            glm::vec3(0, 1, 0), glm::vec3(-1, 2, 3)
        };
        for (int i = 0; i < 4; ++i) {
            nearlyVec(apply(s, probes[i]),
                      glm::vec3(expected * glm::vec4(probes[i], 1.0f)),
                      "combined transform matches T * Rx * Ry * Rz * S");
        }
    }

    // --- the matrix must stay affine ---------------------------------------
    {
        TestShape s;
        s.setPosition(3.0f, 1.0f, -2.0f);
        s.setRotation(15.0f, 25.0f, 35.0f);
        s.setScale(2.0f);
        const glm::mat4 m = s.getModelMatrix();

        ++checks;
        if (std::fabs(m[0][3]) > 1e-6f || std::fabs(m[1][3]) > 1e-6f ||
            std::fabs(m[2][3]) > 1e-6f || std::fabs(m[3][3] - 1.0f) > 1e-6f) {
            ++failures;
            std::printf("  FAIL  the bottom row must be (0, 0, 0, 1) -- "
                        "a model matrix is affine, never projective\n");
        }

        // The translation column is the position, untouched by rotation or
        // scale. If it is not, T was not the outermost factor.
        nearlyVec(glm::vec3(m[3]), glm::vec3(3.0f, 1.0f, -2.0f),
                  "the translation column is the position exactly");
    }

    std::printf("\n%d checks, %d failures\n", checks, failures);
    if (failures == 0) { std::printf("ALL TESTS PASSED (0 failures)\n"); return 0; }
    std::printf("TESTS FAILED (%d failures)\n", failures);
    return 1;
}
