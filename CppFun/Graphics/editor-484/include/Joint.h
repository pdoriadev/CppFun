#ifndef JOINT_H
#define JOINT_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

class Joint {
public:
    Joint();
    ~Joint();

    // Accessors for transformations
    glm::mat4 getTransform() const;
    void setTransform(const glm::mat4& newTransform);

    glm::mat4 getBindWorldToJointTransform() const;
    void setBindWorldToJointTransform(const glm::mat4& newTransform);

    glm::mat4 getCurrentJointToWorldTransform() const;
    void setCurrentJointToWorldTransform(const glm::mat4& newTransform);

    // Getter and setter for rotation
    glm::vec3 getRotation() const;
    void setRotation(const glm::vec3& newRotation);

    // Child joint management
    void addChild(Joint* child);
    const std::vector<Joint*>& getChildren() const;
    Joint* getChild(int index) const;

private:
    glm::vec3 rotation; // Stores current rotation (x, y, z)

    glm::mat4 transform; // Transform relative to its parent
    std::vector<Joint*> children; // List of children

    glm::mat4 bindWorldToJointTransform; // Maps world space to joint space in bind position
    glm::mat4 currentJointToWorldTransform; // Maps joint space to world space in current pose

};

#endif // JOINT_H
