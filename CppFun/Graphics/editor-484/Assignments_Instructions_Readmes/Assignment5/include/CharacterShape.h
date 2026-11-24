#ifndef CHARACTERSHAPE_H
#define CHARACTERSHAPE_H

#include "Shape.h"
#include "SkeletalModel.h"

class CharacterShape : public Shape {
public:
    CharacterShape(float x, float y, float z, float scale, int colorIndex, int id);
    void draw() override;

private:
    SkeletalModel characterModel;
};

#endif
