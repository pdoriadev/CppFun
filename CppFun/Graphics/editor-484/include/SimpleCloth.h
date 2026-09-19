#ifndef SIMPLECLOTH_H
#define SIMPLECLOTH_H

#include "PendulumSystem.h"

class SimpleCloth : public PendulumSystem {
public:
    const char* serialType() const override { return "SimpleCloth"; }
    int getClothSize() const { return m_clothSize; }
    // Constructor
    SimpleCloth(float x, float y, float z, float scale, int colorIndex, int id, float length, float mass, int size);

private:
    int m_clothSize;   // grid dimension, needed to rebuild from a scene file
};

#endif // SIMPLECLOTH_H
