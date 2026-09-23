#include "../include/Cube.h"
#include "../include/TexCoords.h"

#include <cmath>
#include <glm/geometric.hpp>

Cube::Cube(float x, float y, float z, float scale, int colorIndex, int id)
	: Shape(x, y, z, scale, colorIndex, id), VAO(0), VBO(0), EBO(0) {
    shapeType = "Cube";  // Set the type as "Cube"

    // Set up OpenGL buffers
    setupCube();
}

Cube::~Cube() {
    // Cleanup OpenGL resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


//-//////////////////////////////////////////////////////////////////
// Sets element index data
// Computes new un-normalized normal for each vertex that is part of the plane. Sets vertex normal data.
// Sets vertex color data. 
bool Cube::constructPlane(PlaneConstructionParams params) {

    switch(params.type) {
        case PlaneType::TOP : {
            //-/////////////////////////////////////////////////////
            // Add TRI indexes into element buffer.
            // TRI 1
            params.elementData.emplace_back(0);
            params.elementData.emplace_back(1);
            params.elementData.emplace_back(2);
            // TRI 2
            params.elementData.emplace_back(2);
            params.elementData.emplace_back(3);
            params.elementData.emplace_back(0);
            
            //-/////////////////////////////////////////////////////
            // Update vert normals
            glm::vec3 planeVerts[4];
            for (uint32_t p = 0; p < 4; ++p) {
                planeVerts[p] = glm::vec3(  params.vertexData[p * params.VALUES_PER_VERT    ], 
                                            params.vertexData[p * params.VALUES_PER_VERT + 1],
                                            params.vertexData[p * params.VALUES_PER_VERT + 2]      );
            }
        
            glm::vec3 const zeroToOne = planeVerts[1] - planeVerts[0];
            glm::vec3 const zerotoThree = planeVerts[3] - planeVerts[0];
            glm::vec3 const topPlaneNormal = glm::cross(zeroToOne, zerotoThree); // CCW right-hand rule
        
            glm::vec3 const currentNormal = glm::vec3(  params.vertexData[3],
                                                        params.vertexData[3 + 1],
                                                        params.vertexData[3 + 2]);
            
            // NOT normalizing until all planes are constructed. 
            glm::vec3 const newNormal = topPlaneNormal + currentNormal;
                
            for (uint32_t p = 0; p < 4; ++p) {
                // Set normal values
                params.vertexData[3 + p * params.VALUES_PER_VERT    ] = newNormal.x;
                params.vertexData[3 + p * params.VALUES_PER_VERT + 1] = newNormal.y;
                params.vertexData[3 + p * params.VALUES_PER_VERT + 2] = newNormal.z;

                // Set color values
                params.vertexData[6 + p * params.VALUES_PER_VERT    ] = 1.0f;
                params.vertexData[6 + p * params.VALUES_PER_VERT + 1] = 1.0f;
                params.vertexData[6 + p * params.VALUES_PER_VERT + 2] = 1.0f;
            }    
        

            break;
        } // scope operator to declare variables beneath label.
        case PlaneType::INTERMEDIATE : {
            // construct intermediate plane. i moves up and down. j movesleft and right. 

            // covers i == 0 edge-case. 
            uint32_t const LOOPS_COMPLETED_I_MINUS_ONE = (params.LOOPS_COMPLETED_I - 1) > params.MAX_LOOPS 
                ? 0 
                : (params.LOOPS_COMPLETED_I - 1);

            uint32_t elementIndices[4];
            elementIndices[0] = LOOPS_COMPLETED_I_MINUS_ONE * params.VERTS_PER_LOOP  + params.LOOP_PROGRESS_J - 1; // top left
            elementIndices[1] = params.LOOPS_COMPLETED_I    * params.VERTS_PER_LOOP  + params.LOOP_PROGRESS_J - 1; // bottom left
            elementIndices[2] = params.LOOPS_COMPLETED_I    * params.VERTS_PER_LOOP  + params.LOOP_PROGRESS_J; // bottom right
            elementIndices[3] = LOOPS_COMPLETED_I_MINUS_ONE * params.VERTS_PER_LOOP  + params.LOOP_PROGRESS_J; // top right
            
            // TRI 1
            params.elementData.emplace_back(elementIndices[0]);
            params.elementData.emplace_back(elementIndices[1]); 
            params.elementData.emplace_back(elementIndices[2]);
            // TRI 2 
            params.elementData.emplace_back(elementIndices[2]);
            params.elementData.emplace_back(elementIndices[3]);
            params.elementData.emplace_back(elementIndices[0]);

            //-/////////////////////////////////////////////////////
            // update the normals.
            // get plane's vertex values
            glm::vec3 planeVerts[4];
            for (uint32_t p = 0; p < 4; ++p) {
                planeVerts[p] = glm::vec3(  params.vertexData[elementIndices[p] * params.VALUES_PER_VERT    ], 
                                            params.vertexData[elementIndices[p] * params.VALUES_PER_VERT + 1],
                                            params.vertexData[elementIndices[p] * params.VALUES_PER_VERT + 2]      );
            }

            // get plane side vectors between vertices.
            glm::vec3 zeroToOne = planeVerts[1] - planeVerts[0];
            glm::vec3 zeroToThree = planeVerts[3] - planeVerts[0];
            // cross product of side vectors. CCW Right-hand.
            glm::vec3 planeNormal = cross(zeroToOne, zeroToThree);
            
            for(uint32_t p = 0; p < 4; ++p) {
                glm::vec3 currentNormal (params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT],
                    params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT + 1],
                    params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT + 2]);
                // Add cross product with current normal to get new normal.
                currentNormal += planeNormal;

                // Set normal to equal new normal value
                params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT] = currentNormal.x;
                params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT + 1] = currentNormal.y;
                params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT + 2] = currentNormal.z;
            }
             
            break;
        }
        case PlaneType::BOTTOM : {
            uint32_t elementIndices[4];
            for (uint32_t k = 0; k < 4; ++k) {
                elementIndices[k] = params.LOOPS_COMPLETED_I * params.VERTS_PER_LOOP  + params.LOOP_PROGRESS_J - (3 - k);
            }

            // add in correct winding order. Adding CW top-facing will add CCW bottom-facing
            // TRI 1
            params.elementData.emplace_back(elementIndices[3]);
            params.elementData.emplace_back(elementIndices[2]);
            params.elementData.emplace_back(elementIndices[1]);
            // TRI 2
            params.elementData.emplace_back(elementIndices[1]);
            params.elementData.emplace_back(elementIndices[0]);
            params.elementData.emplace_back(elementIndices[3]);

            //-/////////////////////////////////////////////////////
            // update the normals.
            // get plane's vertex values
            glm::vec3 planeVerts[4];
            for (uint32_t p = 0; p < 4; ++p) {
                planeVerts[p] = glm::vec3(  params.vertexData[elementIndices[p] * params.VALUES_PER_VERT    ], 
                                            params.vertexData[elementIndices[p] * params.VALUES_PER_VERT + 1],
                                            params.vertexData[elementIndices[p] * params.VALUES_PER_VERT + 2]      );
            }

            // compute plane normal
            glm::vec3 threeToTwo = planeVerts[2] - planeVerts[3];
            glm::vec3 threeToZero = planeVerts[3] - planeVerts[0];
            glm::vec3 planeNormal = cross(threeToTwo, threeToZero);

            // set normal values (un-normalized)
            for (uint32_t p = 0; p < 4; ++p) {
                glm::vec3 currentNormal(params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT    ],
                                        params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT + 1],
                                        params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT + 2]   );

                currentNormal += planeNormal;
                params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT    ] = currentNormal.x;
                params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT + 1] = currentNormal.y;
                params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT + 2] = currentNormal.z;
            }

            // set color
            for (uint32_t p = 0; p < 4; ++p) {
                params.vertexData[6 + elementIndices[p] * params.VALUES_PER_VERT    ] = 0.7f;
                params.vertexData[6 + elementIndices[p] * params.VALUES_PER_VERT + 1] = 0.7f;
                params.vertexData[6 + elementIndices[p] * params.VALUES_PER_VERT + 2] = 0.7f;
            }
            
            break;
        }
        default:
            // ERROR
            return false;
    }
    
    return true;
}

//-//////////////////////////////////////////////////////////////
//
bool Cube::setupCube() {
    texCoords.clear();

    std::vector<float> vertexData;
    std::vector<unsigned int> elementData;
    uint32_t const VALUES_PER_VERT = 9;
    {
        vertexData.reserve(8 * VALUES_PER_VERT); // 8 vertices per cube * 3 pos per vert * 3 normal per vert * 3 color per vert
        elementData.reserve(36); // 6 sides * 2 triangles per side * 3 vertices per triangle
    }
    
    uint32_t const MAX_LOOPS = 2;
    uint32_t const VERTS_PER_LOOP = 4;
    for(uint32_t i = 0; i < MAX_LOOPS; ++i) {

        float const LOOP_HEIGHT = 0.5*scaleY - (static_cast<float>(i) / (MAX_LOOPS - 1)) * scaleY;
        uint32_t const FULL_LOOP_OFFSET = i * VERTS_PER_LOOP * VALUES_PER_VERT;
        
        for (uint32_t j = 0; j < VERTS_PER_LOOP; ++j) {

            //-//////////////////////////////////////////////////////////////
            // CREATE VERTS. 
            //-//////////////////////////////////////////////////////////////

            if (j == 0) { // first vert of new loop
                vertexData.emplace(vertexData.begin() + FULL_LOOP_OFFSET    , 0.5 * -scaleX);
                vertexData.emplace(vertexData.begin() + FULL_LOOP_OFFSET + 1, LOOP_HEIGHT);
                vertexData.emplace(vertexData.begin() + FULL_LOOP_OFFSET + 2, 0.5 * -scaleZ);
                continue; // need more verts to make a plane.
            }
            
            uint32_t const LAST_VERTEX_INDEX = FULL_LOOP_OFFSET + VALUES_PER_VERT * (j - 1);
            uint32_t const THIS_VERTEX_INDEX = FULL_LOOP_OFFSET + VALUES_PER_VERT * j;
            // Create next vertex
            {
                // derive new vertex's values
                float x = vertexData[LAST_VERTEX_INDEX] // last vert's x
                        + std::cos((PI() * static_cast<double>(j)) / static_cast<double>(VERTS_PER_LOOP)); // new x = last x shifted
                float y = LOOP_HEIGHT; // y is constant for horizontal loop.
                float z = vertexData[LAST_VERTEX_INDEX + 2] // last vert's z
                        + std::sin((PI() * static_cast<double>(j)) / static_cast<double>(VERTS_PER_LOOP)); // new z = last z shifted
                
                // assign vertex values
                vertexData.emplace(vertexData.begin() + THIS_VERTEX_INDEX    , x);
                vertexData.emplace(vertexData.begin() + THIS_VERTEX_INDEX + 1, y);
                vertexData.emplace(vertexData.begin() + THIS_VERTEX_INDEX + 2, z);
            }

            //-//////////////////////////////////////////////////////////////
            // CONSTRUCT PLANES. Add indexes and non-positional vertex data
            //-//////////////////////////////////////////////////////////////

            if (i == 0) { 
                if (j == VERTS_PER_LOOP - 1) { // finished setting loop's vertex position data.
                    // construct top plane. 
                    constructPlane(PlaneConstructionParams(PlaneType::TOP, MAX_LOOPS, VALUES_PER_VERT, VERTS_PER_LOOP, i, j, elementData, vertexData));
                }

                continue;
            }

            // construct intermediate plane. 
            constructPlane(PlaneConstructionParams(PlaneType::INTERMEDIATE, MAX_LOOPS, VALUES_PER_VERT, VERTS_PER_LOOP, i, j, elementData, vertexData));
            
            if (i < MAX_LOOPS - 1) continue;
            if (j < VERTS_PER_LOOP - 1) continue;
            
            // construct bottom plane
            constructPlane(PlaneConstructionParams(PlaneType::BOTTOM, MAX_LOOPS, VALUES_PER_VERT, VERTS_PER_LOOP, i, j, elementData, vertexData));
            
        }
    }

    // TODO(geometry): the cube: eight corners, six faces, twelve triangles
    // Build the shape: fill `vertices`, `faces`, and `normals` (directly or
    // by calling calculateNormals()). See ASSIGNMENTS.md, A2, for the
    // conventions -- roughly one unit across, centred on the origin,
    // counter-clockwise winding seen from outside, every face index a valid
    // index into `vertices`.
    //
    // What is here is a placeholder: a single square in the XY plane. It is
    // deliberately not the shape you were asked for -- it is here so the
    // editor runs, the Insert menu does something visible, and you can see
    // your geometry replace it as you write it. Read src/Torus.cpp first;
    // it is the worked example of a procedural shape.
    // std::vector<std::vector<float>> planePositions
    //          = { {-0.5f, -0.5f, 0.0f}, { 0.5f, -0.5f, 0.0f},
    //              { 0.5f,  0.5f, 0.0f}, {-0.5f,  0.5f, 0.0f} };
    // normals  = { {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
    //              {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} };
    // faces    = { {0, 1, 2}, {0, 2, 3} };

    // UVs are per triangle corner, not per vertex: a cube's eight corners each
    // belong to three faces wanting three different UVs. The loop below emits
    // one vertex per corner, so a per-corner map lines up with it exactly. 
    texCoords = TexCoords::cube(vertices, faces, normals);

    // Create and bind VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementData.size() * sizeof(unsigned int), elementData.data(), GL_STATIC_DRAW);

    // Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float))); // Color
    glEnableVertexAttribArray(2);

    // UVs go in their own buffer on attribute 3, while the VAO is still bound.
    uploadTexCoords();

    glBindVertexArray(0); // Unbind VAO

    return true;
}


void Cube::draw(GLuint shaderProgram) {

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

    // Render the cube
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(faces.size() * 3), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Disable lighting after drawing the cube (for axis rendering)
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 0);
    }
}

