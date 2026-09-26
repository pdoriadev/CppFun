#include "../include/Cube.h"
#include "../include/TexCoords.h"

#include <cstdint>
#include <iostream>
#include <cmath>
#include <glm/geometric.hpp>
#include <sys/types.h>

#pragma region DEBUG_UTILITY
//-///////////////////////////////////////////////
// See "Catching errors (the easy way)" here: https://wikis.khronos.org/opengl/OpenGL_Error
// Also: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDebugMessageCallback.xhtml
void GLAPIENTRY MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    std::string callbackStr = type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "";
    //fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    //       callbackStr.c_str(), type, severity, message);

    std::string messageStr = message;
    std::cout << 
        "\n\tGL CALLBACK: " + callbackStr + 
                    "\n\t\ttype = 0x" + std::to_string(type) + 
                    "\n\t\tseverity = 0x" + std::to_string(severity) +
                    "\n\t\tmessage = " + messageStr << std::endl;
}
#pragma endregion =====================================================================================================================


Cube::Cube(float x, float y, float z, float scale, int colorIndex, int id)
	: Shape(x, y, z, scale, colorIndex, id), VAO(0), VBO(0), EBO(0) {
    shapeType = "Cube";  // Set the type as "Cube"

    // Enable debug output.
    glEnable( GL_DEBUG_OUTPUT );
    // specify debug callback
    glDebugMessageCallback( MessageCallback, 0 );

    // Set up OpenGL buffers
    if (setupCube() == false) std::cerr << "FAILED TO SETUP CUBE" << std::endl;
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
bool Shape::constructPlane(PlaneConstructionParams params) {
    std::cout << "Construct Plane: " << getPlaneTypeString(params.type) << std::endl;

    if (params.type != PlaneType::TOP && params.LOOPS_COMPLETED_I == 0) {
        std::cerr << "Impossible parameters for algorithm. Loop hasn't completed when constructing the top. First loop has completed when constructed any other plane." << std::endl;
        return false;
    }

    uint32_t elementIndices[4];
    glm::vec3 planeVertPositions[4]; 
    glm::vec3 planeNormal = glm::vec3(INFINITY);

    // Compute indices for cube-plane
    switch(params.type) {
        case PlaneType::TOP : 
            elementIndices[0] = 0;
            elementIndices[1] = 1;
            elementIndices[2] = 2;
            elementIndices[3] = 3;
            
            break;
        // scope operator to declare variables beneath label.
        case PlaneType::INTERMEDIATE ... PlaneType::WRAP_AROUND_LOOP : {
            // construct intermediate plane. i moves up and down. j movesleft and right. 
            uint32_t const LOOPS_COMPLETED_I_MINUS_ONE = params.LOOPS_COMPLETED_I - 1;
            uint32_t const J_OFFSET_01 = params.LOOP_PROGRESS_J - (PlaneType::INTERMEDIATE == params.type); // -1 offset
            uint32_t const J_OFFSET_23 = params.LOOP_PROGRESS_J * (PlaneType::INTERMEDIATE == params.type); // + Loop Progress

            elementIndices[0] = LOOPS_COMPLETED_I_MINUS_ONE * params.VERTS_PER_LOOP  + J_OFFSET_01; // top left
            elementIndices[1] = params.LOOPS_COMPLETED_I    * params.VERTS_PER_LOOP  + J_OFFSET_01; // bottom left
            elementIndices[2] = params.LOOPS_COMPLETED_I    * params.VERTS_PER_LOOP  + J_OFFSET_23; // bottom right
            elementIndices[3] = LOOPS_COMPLETED_I_MINUS_ONE * params.VERTS_PER_LOOP  + J_OFFSET_23; // top right
        }     
         
            break;
        case PlaneType::BOTTOM : 
            // CW from top-view is CCW from bottom-view. 
            for (uint32_t k = 0; k < params.VERTS_PER_LOOP ; ++k) {
                elementIndices[k] = params.LOOPS_COMPLETED_I * params.VERTS_PER_LOOP + params.LOOP_PROGRESS_J - k;
            }
            
            break;
        default: // ERROR. Invalid case. 
            std::cerr << "INVALID PLANE TYPE FOR CUBE PLANE: " << getPlaneTypeString(params.type) << std::endl;
            return false;
    }

    // TRI 1
    params.elementData.emplace_back(elementIndices[0]);
    params.elementData.emplace_back(elementIndices[1]);
    params.elementData.emplace_back(elementIndices[2]);
    // TRI 2
    params.elementData.emplace_back(elementIndices[2]);
    params.elementData.emplace_back(elementIndices[3]);
    params.elementData.emplace_back(elementIndices[0]);
    std::cout << "ELEMENTS OUTPUT" << std::endl;
    outputElements(params.elementData, 3);
    std::cout << "ELEMENTS OUTPUT COMPLETE" << std::endl;
    
    //-/////////////////////////////////////////////////////
    // COMPUTE NORMALS. ADD COLORS.
    //-/////////////////////////////////////////////////////
    
    for (uint32_t p = 0; p < 4; ++p) {
        planeVertPositions[p] = glm::vec3(  params.vertexData[elementIndices[p] * params.VALUES_PER_VERT    ], 
                                            params.vertexData[elementIndices[p] * params.VALUES_PER_VERT + 1],
                                            params.vertexData[elementIndices[p] * params.VALUES_PER_VERT + 2]      );
    }

    glm::vec3 const zeroToOne = planeVertPositions[1] - planeVertPositions[0];
    glm::vec3 const zeroToThree = planeVertPositions[3] - planeVertPositions[0]; 
    // if (params.type == PlaneType::BOTTOM) planeNormal = glm::cross(zeroToThree, zeroToOne); 
    planeNormal = glm::cross(zeroToOne, zeroToThree); 
    // // Compute new plane's normal
    // switch(params.type) {
    //     case PlaneType::TOP ... PlaneType::WRAP_AROUND_LOOP : 
    //         planeNormal = glm::cross(zeroToThree, zeroToOne); break; // CCW right-hand rule 
    //     case PlaneType::BOTTOM : 
    //         planeNormal = glm::cross(zeroToOne, zeroToThree); break;
    //     default: // ERROR
    //         return false;
    // }
    
    for(uint32_t p = 0; p < 4; ++p) {
        glm::vec3 normal (  params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT    ],
                            params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT + 1],
                            params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT + 2]   );
        // Add cross product with current normal to get new normal.
        normal += planeNormal;

        // Set normal to equal new normal value
        params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT] = normal.x;
        params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT + 1] = normal.y;
        params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERT + 2] = normal.z;

        // Set color values
        float const newColor = static_cast<float>(elementIndices[p]) / static_cast<float>(params.VERTS_PER_LOOP * params.MAX_LOOPS);
        params.vertexData[6 + elementIndices[p] * params.VALUES_PER_VERT    ] = newColor;
        params.vertexData[6 + elementIndices[p] * params.VALUES_PER_VERT + 1] = newColor;
        params.vertexData[6 + elementIndices[p] * params.VALUES_PER_VERT + 2] = newColor;
    }

    outputVertices(params.vertexData, params.VALUES_PER_VERT);

    std::cout << "COMPLETED PLANE CONSTRUCTION" << std::endl;


    return true;
}

//-//////////////////////////////////////////////////////////////
//
bool Cube::setupCube() {

    std::cout << "SETTINGUP CUBE" << std::endl;
    
    uint32_t const VALUES_PER_VERT = 9; // 3 pos + 3 norm + 3 col
    std::vector<float> vertexData;
    vertexData.reserve(8 * VALUES_PER_VERT); // 8 vertices per cube

    std::vector<unsigned int> elementData;
    elementData.reserve(36); // 6 sides * 2 triangles per side * 3 verts per triangle

    uint32_t const MAX_LOOPS = 2;
    uint32_t const VERTS_PER_LOOP = 4;
    
    for(uint32_t i = 0; i < MAX_LOOPS; ++i) {
        std::cout << "LOOP " << i << std::endl;

        float const LOOP_HEIGHT = 0.5*scale - (static_cast<float>(i) / (MAX_LOOPS - 1)) * scale;
        uint32_t const FULL_LOOP_OFFSET = i * VERTS_PER_LOOP * VALUES_PER_VERT;
        
        for (uint32_t j = 0; j < VERTS_PER_LOOP; ++j) {
            std::cout << "VERT THIS LOOP: " << j << std::endl;

            //-//////////////////////////////////////////////////////////////
            // CREATE VERTS. 
            //-//////////////////////////////////////////////////////////////

            if (j == 0) { // first vert of new loop
                std::cout << "FIRST VERTEX: " << j << std::endl;
                float const x = 0.5 * -scale;
                float const y = LOOP_HEIGHT;
                float const z = 0.5 * -scale;
                vertexData.push_back(x);
                vertexData.push_back(y);
                vertexData.push_back(z);

                vertices.push_back(glm::vec3(x, y , z));

                // assign dummy values to normal and color. These will be changed later. 
                for (uint8_t k = 0; k < VALUES_PER_VERT - 3; ++k) {
                    vertexData.push_back(0.0f);
                }
                outputVertices(vertexData, VALUES_PER_VERT);

                continue; // need more verts to make a plane.
            }
            
            uint32_t const LAST_VERTEX_INDEX = FULL_LOOP_OFFSET + VALUES_PER_VERT * (j - 1);
            uint32_t const THIS_VERTEX_INDEX = FULL_LOOP_OFFSET + VALUES_PER_VERT * j;

            std::cout << "create next vertex" << j << std::endl;
            std::cout << "LAST_VERTEX_INDEX: " << LAST_VERTEX_INDEX << std::endl;
            std::cout << "THIS_VERTEX_INDEX: " << THIS_VERTEX_INDEX << std::endl;

            // Create next vertex
            {
                double trigInputForOffset = 2 * ShapeMath::PI() * (static_cast<double>(j - 1) / static_cast<double>(VERTS_PER_LOOP));

                // derive new vertex's values
                float const x = vertexData.at(LAST_VERTEX_INDEX) + scale * std::cos(trigInputForOffset); // new x = last x shifted
                // std::cout << "Computed x: " << x << std::endl;
                float const y = LOOP_HEIGHT; // y is constant for horizontal loop.
                // std::cout << "Computed y: " << y << std::endl;
                float const z = vertexData.at(LAST_VERTEX_INDEX + 2) + scale * std::sin(trigInputForOffset); // new z = last z shifted
                // std::cout << "Computed z: " << z << std::endl;
                
                // std::cout << "computed vertex values" << std::endl;
                // assign vertex values
                vertexData.push_back(x);
                vertexData.push_back(y);
                vertexData.push_back(z);
                // assign dummy values to normal and color. These will be changed later. 
                for (uint8_t k = 0; k < VALUES_PER_VERT - 3; ++k) {
                    vertexData.push_back(0.0f);
                }

                outputVertices(vertexData, VALUES_PER_VERT);
            }

            //-//////////////////////////////////////////////////////////////
            // CONSTRUCT PLANES. 
            //-//////////////////////////////////////////////////////////////
            
            std::vector<PlaneType> planeTypes;
            planeTypes.reserve(3); // max of 3 planes constructed per cube

            // Always add a side plane when i > 0 and j > 0. 
            if (i > 0) { planeTypes.push_back(PlaneType::INTERMEDIATE); }
            
            if (j == VERTS_PER_LOOP - 1) {
                // loop-completion planes. Tops, Bottoms, Wrap-arounds. 
                switch(i) {
                    case 0:                     
                        planeTypes.push_back(PlaneType::TOP); 
                        break;
                    case 1 ... MAX_LOOPS - 1:   
                        planeTypes.push_back(PlaneType::WRAP_AROUND_LOOP);
                        if (i == MAX_LOOPS - 1) { 
                            planeTypes.push_back(PlaneType::BOTTOM); 
                        }
                        break;
                    default:
                        std::cerr << "INVALID i VALUE in Cube::setupCube(): " << i << std::endl;
                        return false;
                }
            }
                        
            for (uint32_t p = 0; p < planeTypes.size(); ++p) {
                constructPlane(PlaneConstructionParams(planeTypes[p], MAX_LOOPS, VERTS_PER_LOOP, VALUES_PER_VERT, i, j, elementData, vertexData));
            }
        }
    }

    // normalize normals.
    // the plane construction algorithm sums a new plane's normal into the old normal value. We normalize at the
    //      end to get correct values we square_root(x^2 + y^2 + z^2);
    for (uint32_t i = 0; i < MAX_LOOPS * VERTS_PER_LOOP; ++i) {
        glm::vec3 normal (vertexData[3 + i * VALUES_PER_VERT],
                        vertexData[3 + i * VALUES_PER_VERT + 1],
                        vertexData[3 + i * VALUES_PER_VERT + 2]);
        normal = normalize(normal);
        vertexData[3 + i * VALUES_PER_VERT] = normal.x;
        vertexData[3 + i * VALUES_PER_VERT + 1] = normal.y;
        vertexData[3 + i * VALUES_PER_VERT + 2] = normal.z;

        normals.reserve(vertexData.size() / 3);
        normals.push_back(normal);
    }

    vertexData.shrink_to_fit();
    outputVertices(vertexData, VALUES_PER_VERT);

    elementData.shrink_to_fit();
    outputElements(elementData, 3);

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

    // texCoords.clear();

    // UVs are per triangle corner, not per vertex: a cube's eight corners each
    // belong to three faces wanting three different UVs. The loop below emits
    // one vertex per corner, so a per-corner map lines up with it exactly. 
    // texCoords = TexCoords::cube(vertices, faces, normals);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VALUES_PER_VERT * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VALUES_PER_VERT * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VALUES_PER_VERT * sizeof(float), (void*)(6 * sizeof(float))); // Color
    glEnableVertexAttribArray(2);

    // UVs go in their own buffer on attribute 3, while the VAO is still bound.
    // uploadTexCoords();

    glBindVertexArray(0); // Unbind VAO

    return true;
}


/* Sphere vertex position. See assignment pdf. 
(i / LatSteps) * 180
(j / LongSteps) * 360
*/

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
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(36), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Disable lighting after drawing the cube (for axis rendering)
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 0);
    }
}

