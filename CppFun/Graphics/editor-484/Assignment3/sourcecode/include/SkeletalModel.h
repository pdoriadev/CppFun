#ifndef SKELETALMODEL_H
#define SKELETALMODEL_H

#include "MatrixStack.h"
#include "Joint.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class SkeletalModel {
public:
    SkeletalModel();

    Joint* getRootJoint() const;
    void setRootJoint(Joint* rootJoint);
    void addJointChild(int parentIndex, Joint* child);

    const std::vector<Joint*>& getJoints() const;
    void setJoints(const std::vector<Joint*>& joints);

    const std::vector<glm::vec3>& getJointCenters() const;
    const std::vector<std::pair<glm::vec3, glm::vec3>> getBonePairs() const;

    MatrixStack& getMatrixStack();

    // Part 1: Understanding Hierarchical Modeling


    // 1.3. Implement this method to handle changes to your skeleton given
    // changes in the slider values
    void setJointTransform(int jointIndex, float rX, float rY, float rZ);

    // Part 2: Skeletal Subspace Deformation

    // 2.3. Implement SSD

    // 2.3.1. Implement this method to compute a per-joint transform from
    // world-space to joint space in the BIND POSE.
    void computeBindWorldToJointTransforms();

    // 2.3.2. Implement this method to compute a per-joint transform from
    // joint space to world space in the CURRENT POSE.
    void updateCurrentJointToWorldTransforms();


private:
    std::vector<Joint*> m_joints;
    Joint* m_rootJoint;
    MatrixStack m_matrixStack;
    std::vector<glm::vec3> jointCenters;
    std::vector<std::pair<glm::vec3, glm::vec3>> bonePairs;    
};

#endif
