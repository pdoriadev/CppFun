#ifndef MOBIUS_H
#define MOBIUS_H

#include "Shape.h"

// A Mobius strip: a rectangle swept once around the Y axis while its
// cross-section rotates a half turn.
//
// Included because it is the clearest demonstration of two-sided shading. The
// surface has a single side and a single edge, so there is no consistent
// "outward" winding to derive normals from - a renderer that decides which way
// a face points from its winding order will render half of this strip black.
// Orienting the shading normal against the view ray instead handles it.
class Mobius : public Shape {
public:
    const char* serialType() const override { return "Mobius"; }

    Mobius(float x, float y, float z, float scale, int colorIndex, int id);
    ~Mobius();

    void draw(GLuint shaderProgram) override;
    void setupMobius();

private:
    GLuint VAO, VBO, EBO;
    int indexCount;
};

#endif // MOBIUS_H
