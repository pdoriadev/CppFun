#ifndef IMPORTCHARACTER_H
#define IMPORTCHARACTER_H

#include "Shape.h"
#include "SkeletalModel.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

class ImportCharacter : public Shape {
public:
    const char* serialType() const override { return "ImportCharacter"; }
    ImportCharacter(float x, float y, float z, float scale, int colorIndex, int id);
    ~ImportCharacter();
    
    enum DisplayMode { SKELETAL, MESH };

    // Getter and setter for skeletal model
    SkeletalModel& getSkeletalModel();
    void setJointTransform(int jointIndex, float rX, float rY, float rZ);

    // Getters and setters for bindVertices
    const std::vector<glm::vec3>& getBindVertices() const;
    void setBindVertices(const std::vector<glm::vec3>& vertices);	

    // Getters and setters for attachments
    const std::vector<std::vector<float>>& getAttachments() const;
    void setAttachments(const std::vector<std::vector<float>>& attachments);

    // Getter and setter for display mode
    DisplayMode getDisplayMode() const;
    void setDisplayMode(DisplayMode mode);

    // Update vertices for SSD
    void updateMeshVertices(); 
    void resetPose();

    void setupMeshBuffer();    
    void setupJointBuffer();    
    void setupBoneBuffer();    


    void draw(GLuint shaderProgram) override;

private:

    // Generate Sphere for Joint
    void generateSphere(float radius, glm::vec3 centerJoint,
                         std::vector<glm::vec3>& outVertices,
                         std::vector<glm::vec3>& outNormals,
                         std::vector<glm::uvec3>& outFaces);

    // Generate Cuboid for Bone
    void generateCuboid(const glm::vec3& parentPos, const glm::vec3& childPos,
                         std::vector<glm::vec3>& boneVertices, std::vector<glm::vec3>& boneNormals,
                         std::vector<glm::uvec3>& boneFaces);

    // Current vertex positions after animation
    std::vector<glm::vec3> bindVertices; // Initial vertex positions

    // List of vertex to joint attachments
    // each element of attachments is a vector< float > containing
    // one attachment weight per joint
    std::vector<std::vector<float>> attachments; // Attachment weights

    SkeletalModel m_skeletalModel;  // Directly owned skeletal model
	
    DisplayMode displayMode = MESH;  // Default to skeletal mode

    GLuint meshVAO, meshVBO, meshEBO;
    GLuint jointVAO, jointVBO, jointEBO;
    GLuint boneVAO, boneVBO, boneEBO;

    float jointIndexCount, boneIndexCount;
};

#endif // IMPORTCHARACTER_H
