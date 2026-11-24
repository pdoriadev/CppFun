#include "Joint.h"

// Constructor initializing with identity matrices
Joint::Joint()
    // Order follows Joint.h. Members are initialised in declaration order
    // whatever this list says, so a mismatch here is a standing invitation for
    // one member to be initialised from another that is still garbage.
    : rotation(glm::vec3(0.0f)),
      transform(glm::mat4(1.0f)),
      bindWorldToJointTransform(glm::mat4(1.0f)),
      currentJointToWorldTransform(glm::mat4(1.0f)) {}

// Destructor to ensure children are deleted
Joint::~Joint() {
    for (auto* child : children) {
        delete child;
    }
}

// Getter and setter for transform
glm::mat4 Joint::getTransform() const {
    return transform;
}

void Joint::setTransform(const glm::mat4& newTransform) {
    transform = newTransform;
}

// Getter and setter for bindWorldToJointTransform
glm::mat4 Joint::getBindWorldToJointTransform() const {
    return bindWorldToJointTransform;
}

void Joint::setBindWorldToJointTransform(const glm::mat4& newTransform) {
    bindWorldToJointTransform = newTransform;
}

// Getter and setter for currentJointToWorldTransform
glm::mat4 Joint::getCurrentJointToWorldTransform() const {
    return currentJointToWorldTransform;
}

void Joint::setCurrentJointToWorldTransform(const glm::mat4& newTransform) {
    currentJointToWorldTransform = newTransform;
}


// Getter for rotation
glm::vec3 Joint::getRotation() const {
    return rotation;
}

// Setter for rotation
void Joint::setRotation(const glm::vec3& newRotation) {
    rotation = newRotation;
}

// Add child joint
void Joint::addChild(Joint* child) {
    children.push_back(child);
}

// Get children joints
const std::vector<Joint*>& Joint::getChildren() const {
    return children;
}

// Get a specific child joint by index
Joint* Joint::getChild(int index) const {
    if (index >= 0 && index < static_cast<int>(children.size())) {
        return children[index];
    } else {
        return nullptr; // Return nullptr if the index is out of bounds
    }
}
