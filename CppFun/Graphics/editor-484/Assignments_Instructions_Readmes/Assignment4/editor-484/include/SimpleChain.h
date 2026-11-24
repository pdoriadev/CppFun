#ifndef SIMPLECHAIN_H
#define SIMPLECHAIN_H

#include "PendulumSystem.h"

class SimpleChain : public PendulumSystem {
public:
    const char* serialType() const override { return "SimpleChain"; }
    // Constructor
    SimpleChain(float x, float y, float z, float scale, int colorIndex, int id, float length, float mass);

};

#endif // SIMPLECHAIN_H
