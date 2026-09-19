#include "CharacterShape.h"

CharacterShape::CharacterShape(float x, float y, float z, float scale, int colorIndex, int id)
    : Shape(x, y, z, scale, colorIndex, id) {
    characterModel.load("skeleton.txt", "mesh.obj", "attachments.txt");
}

void CharacterShape::draw() {
    Matrix4f cameraMatrix = Matrix4f::identity();  // Replace with actual camera matrix
    bool skeletonVisible = true;  // Option to toggle skeleton visibility
    characterModel.draw(cameraMatrix, skeletonVisible);
}
