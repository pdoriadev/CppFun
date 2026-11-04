#include "ImportCharacter.h"
#include <iostream>

ImportCharacter::ImportCharacter(float x, float y, float z, float scale, int colorIndex, int id)
    : Shape(x, y, z, scale, colorIndex, id), m_skeletalModel(),
      meshVAO(0), meshVBO(0), meshEBO(0), 
      jointVAO(0), jointVBO(0), jointEBO(0), 
      boneVAO(0), boneVBO(0), boneEBO(0),
      jointIndexCount(0), boneIndexCount(0) {

}

ImportCharacter::~ImportCharacter() {
    glDeleteVertexArrays(1, &meshVAO);
    glDeleteBuffers(1, &meshVBO);
    glDeleteBuffers(1, &meshEBO);

    glDeleteVertexArrays(1, &jointVAO);
    glDeleteBuffers(1, &jointVBO);
    glDeleteBuffers(1, &jointEBO);

    glDeleteVertexArrays(1, &boneVAO);
    glDeleteBuffers(1, &boneVBO);
    glDeleteBuffers(1, &boneEBO);
}

void ImportCharacter::setupMeshBuffer() {

    // Clear existing data
    if (meshVAO) glDeleteVertexArrays(1, &meshVAO);
    if (meshVBO) glDeleteBuffers(1, &meshVBO);
    if (meshEBO) glDeleteBuffers(1, &meshEBO);

    // Collect vertices, normals, colors, and indices
    std::vector<float> meshVertices;
    std::vector<unsigned int> meshIndices;

    for (size_t i = 0; i < faces.size(); ++i) {
    
        glm::vec3 normal = normals[i]; // Assign face normal
        glm::vec3 color = (colorIndex == 31) 
            ? glm::vec3(
                customColor[0], 
                customColor[1], 
                customColor[2]
            )
            : glm::vec3(
                colorPresets[colorIndex].color[0], 
                colorPresets[colorIndex].color[1], 
                colorPresets[colorIndex].color[2]
             );

        for (int j = 0; j < 3; ++j) {
            int vertexIndex = faces[i][j];
            const glm::vec3& position = vertices[vertexIndex];

            // Append position, normal, and color to meshVertices
            meshVertices.insert(meshVertices.end(), {position.x, position.y, position.z});
            meshVertices.insert(meshVertices.end(), {normal.x, normal.y, normal.z});
            meshVertices.insert(meshVertices.end(), {color.r, color.g, color.b});
        }

        meshIndices.insert(meshIndices.end(), {static_cast<unsigned int>(i * 3), static_cast<unsigned int>(i * 3 + 1), static_cast<unsigned int>(i * 3 + 2)});
    }

    // Create and bind meshVAO, meshVBO, and meshEBO
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &meshVBO);
    glGenBuffers(1, &meshEBO);

    glBindVertexArray(meshVAO);

    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    glBufferData(GL_ARRAY_BUFFER, meshVertices.size() * sizeof(float), meshVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshIndices.size() * sizeof(unsigned int), meshIndices.data(), GL_STATIC_DRAW);

    // Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float))); // Color
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind meshVAO
}



void ImportCharacter::setupJointBuffer() {

    // Clear existing data
    if (jointVAO) glDeleteVertexArrays(1, &jointVAO);
    if (jointVBO) glDeleteBuffers(1, &jointVBO);
    if (jointEBO) glDeleteBuffers(1, &jointEBO);

    // Retrieve center of joints
    const auto& jointCenters = m_skeletalModel.getJointCenters();

    // Collect vertices, normals, colors, and indices
    std::vector<glm::vec3> jointVertices;
    std::vector<glm::vec3> jointNormals;
    std::vector<glm::uvec3> jointFaces;

    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;

    // Generate the sphere for each joint center position
    for (const Joint* joint : m_skeletalModel.getJoints()) {
        glm::vec3 center = glm::vec3(joint->getCurrentJointToWorldTransform()[3]);
        generateSphere(0.02f, center, jointVertices, jointNormals, jointFaces);
    }

    for (size_t i = 0; i < jointFaces.size(); ++i) {
        for (int j = 0; j < 3; ++j) {
    
            int vertexIndex = jointFaces[i][j];
            const glm::vec3& position = jointVertices[vertexIndex];
            const glm::vec3& normal = jointNormals[vertexIndex];
            glm::vec3 color = glm::vec3(1.0f); // white
        
            // Append position, normal, and color to vertexData
            vertexData.insert(vertexData.end(), {position.x, position.y, position.z});
            vertexData.insert(vertexData.end(), {normal.x, normal.y, normal.z});
            vertexData.insert(vertexData.end(), {color.r, color.g, color.b});
        }

        indexData.insert(indexData.end(), {static_cast<unsigned int>(i * 3), static_cast<unsigned int>(i * 3 + 1), static_cast<unsigned int>(i * 3 + 2)});
    }

    jointIndexCount = static_cast<GLsizei>(indexData.size());

    // Create and bind VAO, VBO, and EBO
    glGenVertexArrays(1, &jointVAO);
    glGenBuffers(1, &jointVBO);
    glGenBuffers(1, &jointEBO);

    glBindVertexArray(jointVAO);

    glBindBuffer(GL_ARRAY_BUFFER, jointVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jointEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);

    // Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float))); // Color
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO
}

void ImportCharacter::setupBoneBuffer() {

     // Clear existing data
    if (boneVAO) glDeleteVertexArrays(1, &boneVAO);
    if (boneVBO) glDeleteBuffers(1, &boneVBO);
    if (boneEBO) glDeleteBuffers(1, &boneEBO);
    
    const auto& joints = m_skeletalModel.getJoints();
    
    // Collect vertices, normals, colors, and indices
    std::vector<glm::vec3> boneVertices;
    std::vector<glm::vec3> boneNormals;
    std::vector<glm::uvec3> boneFaces;
     

    for (const Joint* parent : joints) {
        glm::vec3 parentPos = glm::vec3(parent->getCurrentJointToWorldTransform()[3]);
        for (const Joint* child : parent->getChildren()) {
            glm::vec3 childPos = glm::vec3(child->getCurrentJointToWorldTransform()[3]);
            generateCuboid(parentPos, childPos, boneVertices, boneNormals, boneFaces);
        }
    }

    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;


    // Construct vertex and index data from generated vertices, normals, and faces
    for (size_t i = 0; i < boneFaces.size(); ++i) {
        for (int j = 0; j < 3; ++j) {
    
            int vertexIndex = boneFaces[i][j];
            const glm::vec3& position = boneVertices[vertexIndex];
            const glm::vec3& normal = boneNormals[vertexIndex];
            glm::vec3 color = glm::vec3(1.0f); // white
        
            // Append position, normal, and color to vertexData
            vertexData.insert(vertexData.end(), {position.x, position.y, position.z});
            vertexData.insert(vertexData.end(), {normal.x, normal.y, normal.z});
            vertexData.insert(vertexData.end(), {color.r, color.g, color.b});
        }

        indexData.insert(indexData.end(), {static_cast<unsigned int>(i * 3), static_cast<unsigned int>(i * 3 + 1), static_cast<unsigned int>(i * 3 + 2)});
    }

    boneIndexCount = static_cast<GLsizei>(indexData.size());

    // Create and bind VAO, VBO, and EBO
    glGenVertexArrays(1, &boneVAO);
    glGenBuffers(1, &boneVBO);
    glGenBuffers(1, &boneEBO);

    glBindVertexArray(boneVAO);

    glBindBuffer(GL_ARRAY_BUFFER, boneVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boneEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    
}



// Getter for bindVertices
const std::vector<glm::vec3>& ImportCharacter::getBindVertices() const {
    return bindVertices;
}

// Setter for bindVertices
void ImportCharacter::setBindVertices(const std::vector<glm::vec3>& vertices) {
    bindVertices = vertices;
}

// Getter for skeletal model
SkeletalModel& ImportCharacter::getSkeletalModel() {
    return m_skeletalModel;
}

// Set joint transform by index
void ImportCharacter::setJointTransform(int jointIndex, float rX, float rY, float rZ) {
    m_skeletalModel.setJointTransform(jointIndex, rX, rY, rZ);
}

// Getter for attachments
const std::vector<std::vector<float>>& ImportCharacter::getAttachments() const {
    return attachments;
}

// Setter for attachments
void ImportCharacter::setAttachments(const std::vector<std::vector<float>>& attachments) {
    this->attachments = attachments;
}

// Getter for display mode
ImportCharacter::DisplayMode ImportCharacter::getDisplayMode() const {
    return displayMode;
}

// Setter for display mode
void ImportCharacter::setDisplayMode(DisplayMode mode) {
    displayMode = mode;
}



void ImportCharacter::draw(GLuint shaderProgram) {


    updateMeshVertices();
    
    if (displayMode == SKELETAL) {
    
        // Use the shader program
        glUseProgram(shaderProgram);
        
        // Apply transformations and pass to the shader
        applyTransform(shaderProgram);

        // Enable lighting for joint and bone visualization
        GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
        if (lightingLoc != -1) {
            glUniform1i(lightingLoc, 1);
        }

        GLint colorLoc = glGetUniformLocation(shaderProgram, "material.color");
        if (colorLoc != -1) {
            glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // White
        }

        // Render the character bones
        glBindVertexArray(boneVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(jointIndexCount), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        // Render the character joints
        glBindVertexArray(jointVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(jointIndexCount), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Disable lighting after drawing the cube (for axis rendering)
        if (lightingLoc != -1) {
            glUniform1i(lightingLoc, 0);
        } 

    } else if (displayMode == MESH) {

        // Use the shader program
        glUseProgram(shaderProgram);
        
        // Enable lighting for the cube
        GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
        if (lightingLoc != -1) {
            glUniform1i(lightingLoc, 1); // Enable lighting for the cube
        }

        // Apply transformations and pass to the shader
        applyTransform(shaderProgram);

        // Pass material properties
        GLint colorLoc = glGetUniformLocation(shaderProgram, "material.color");
        if (colorLoc != -1) {
            glUniform3fv(colorLoc, 1, (colorIndex == 31) ? customColor : colorPresets[colorIndex].color);
        }

        // Render the character mesh
        glBindVertexArray(meshVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(faces.size() * 3), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Disable lighting after drawing the cube (for axis rendering)
        if (lightingLoc != -1) {
            glUniform1i(lightingLoc, 0);
        }
        
    }
}


void ImportCharacter::resetPose() {
    for (size_t i = 0; i < m_skeletalModel.getJoints().size(); ++i) {
        m_skeletalModel.setJointTransform(i, 0.0f, 0.0f, 0.0f);
    }

    m_skeletalModel.updateCurrentJointToWorldTransforms();
    updateMeshVertices();
}



void ImportCharacter::generateSphere(float radius, glm::vec3 center,
                         std::vector<glm::vec3>& outVertices,
                         std::vector<glm::vec3>& outNormals,
                         std::vector<glm::uvec3>& outFaces) {
    const unsigned int latitudeSegments = 6;
    const unsigned int longitudeSegments = 6;

    unsigned int startIndex = outVertices.size();

    for (unsigned int y = 0; y <= latitudeSegments; ++y) {
        for (unsigned int x = 0; x <= longitudeSegments; ++x) {
            float xSegment = static_cast<float>(x) / longitudeSegments;
            float ySegment = static_cast<float>(y) / latitudeSegments;

            float xPos = radius * std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
            float yPos = radius * std::cos(ySegment * M_PI);
            float zPos = radius * std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);

            glm::vec3 localPos = glm::vec3(xPos, yPos, zPos);
            outVertices.push_back(center + localPos);                // translate to center
            outNormals.push_back(glm::normalize(localPos));          // normal from sphere origin
        }
    }

    // Generate triangle faces
    for (unsigned int y = 0; y < latitudeSegments; ++y) {
        for (unsigned int x = 0; x < longitudeSegments; ++x) {
            unsigned int first = startIndex + y * (longitudeSegments + 1) + x;
            unsigned int second = first + longitudeSegments + 1;

            outFaces.push_back({first, second, first + 1});
            outFaces.push_back({second, second + 1, first + 1});
        }
    }
}


void ImportCharacter::generateCuboid(
    const glm::vec3& parentPos, const glm::vec3& childPos,
    std::vector<glm::vec3>& outVertices,
    std::vector<glm::vec3>& outNormals,
    std::vector<glm::uvec3>& outFaces
) {
    // Direction and length from parent to child
    glm::vec3 dir = childPos - parentPos;
    float length = glm::length(dir);
    if (length < 1e-6f) return;

    glm::vec3 z = glm::normalize(dir);
    glm::vec3 tmp = glm::vec3(0, 1, 0);
    if (glm::abs(glm::dot(z, tmp)) > 0.99f) tmp = glm::vec3(1, 0, 0);
    glm::vec3 x = glm::normalize(glm::cross(tmp, z));
    glm::vec3 y = glm::normalize(glm::cross(z, x));

    glm::mat4 rotation = glm::mat4(
        glm::vec4(x, 0),
        glm::vec4(y, 0),
        glm::vec4(z, 0),
        glm::vec4(0, 0, 0, 1)
    );

    glm::vec3 center = (parentPos + childPos) * 0.5f;
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), center);

    glm::vec3 halfSize(0.01f, 0.01f, length * 0.5f);

    glm::vec3 localVerts[] = {
        {-halfSize.x, -halfSize.y, -halfSize.z},
        { halfSize.x, -halfSize.y, -halfSize.z},
        { halfSize.x,  halfSize.y, -halfSize.z},
        {-halfSize.x,  halfSize.y, -halfSize.z},
        {-halfSize.x, -halfSize.y,  halfSize.z},
        { halfSize.x, -halfSize.y,  halfSize.z},
        { halfSize.x,  halfSize.y,  halfSize.z},
        {-halfSize.x,  halfSize.y,  halfSize.z}
    };

    glm::vec3 faceNormals[] = {
        { 0,  0, -1}, { 0,  0, 1},
        { 0, -1,  0}, { 0,  1, 0},
        {-1,  0,  0}, { 1,  0, 0}
    };

    int faceTriangles[12][3] = {
        // Back face
        {2, 1, 0}, {0, 3, 2},
        // Front face
        {4, 5, 6}, {4, 6, 7},
        // Bottom face
        {0, 1, 5}, {0, 5, 4},
        // Top face
        {6, 2, 3}, {7, 6, 3},
        // Left face
        {7, 3, 0}, {4, 7, 0},
        // Right face
        {1, 2, 6}, {1, 6, 5}
    };
    
    unsigned int offset = static_cast<unsigned int>(outVertices.size());

    for (int t = 0; t < 12; ++t) {
        int i0 = faceTriangles[t][0];
        int i1 = faceTriangles[t][1];
        int i2 = faceTriangles[t][2];

        glm::vec3 p0 = glm::vec3(translate * rotation * glm::vec4(localVerts[i0], 1.0f));
        glm::vec3 p1 = glm::vec3(translate * rotation * glm::vec4(localVerts[i1], 1.0f));
        glm::vec3 p2 = glm::vec3(translate * rotation * glm::vec4(localVerts[i2], 1.0f));

        // Position
        outVertices.push_back(p0);
        outVertices.push_back(p1);
        outVertices.push_back(p2);

        // Compute normal from triangle (better than hardcoding)
        glm::vec3 normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));

        // Rotate if needed (here already in world space, so no rotation needed)
        outNormals.push_back(normal);
        outNormals.push_back(normal);
        outNormals.push_back(normal);

        outFaces.push_back(glm::uvec3(offset, offset + 1, offset + 2));
        offset += 3;
    }
}






