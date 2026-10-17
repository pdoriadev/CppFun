#include "SkeletalModel.h"

SkeletalModel::SkeletalModel() {}

// Getters and setters for root joint
Joint* SkeletalModel::getRootJoint() const { return m_rootJoint; }
void SkeletalModel::setRootJoint(Joint* rootJoint) { m_rootJoint = rootJoint; }

// Getters and setters for all joints
const std::vector<Joint*>& SkeletalModel::getJoints() const { return m_joints; }
void SkeletalModel::setJoints(const std::vector<Joint*>& joints) { m_joints = joints; }

// Getter for joint centers and bone pairs
const std::vector<glm::vec3>& SkeletalModel::getJointCenters() const { return jointCenters; }
const std::vector<std::pair<glm::vec3, glm::vec3>> SkeletalModel::getBonePairs() const { return bonePairs; }

MatrixStack& SkeletalModel::getMatrixStack() { return m_matrixStack; }

void SkeletalModel::addJointChild(int parentIndex, Joint* child) {
    if (parentIndex < 0 || parentIndex >= static_cast<int>(m_joints.size())) {
        std::cerr << "Error: Invalid parent index provided." << std::endl;
        return;
    }

    Joint* parent = m_joints[parentIndex];
    parent->addChild(child);
    m_joints.push_back(child);
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ) {

    if (jointIndex < 0 || jointIndex >= static_cast<int>(m_joints.size())) {
        std::cerr << "Invalid joint index" << std::endl;
        return;
    }

    Joint* joint = m_joints[jointIndex];

    // Store the rotation as Euler angles
    joint->setRotation(glm::vec3(rX, rY, rZ));

    // Create a rotation matrix from Euler angles
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rX), glm::vec3(1, 0, 0)) *
                               glm::rotate(glm::mat4(1.0f), glm::radians(rY), glm::vec3(0, 1, 0)) *
                               glm::rotate(glm::mat4(1.0f), glm::radians(rZ), glm::vec3(0, 0, 1));

    // Combine with the current transform: preserve translation, update rotation
    glm::mat4 newTransform = joint->getTransform();

    // Copy upper-left 3×3 rotation part into existing matrix
    for (int row = 0; row < 3; ++row)
        for (int col = 0; col < 3; ++col)
            newTransform[col][row] = rotationMatrix[col][row];

    joint->setTransform(newTransform);
    
}






