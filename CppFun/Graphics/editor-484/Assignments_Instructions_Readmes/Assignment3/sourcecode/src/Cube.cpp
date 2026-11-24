#include "../include/Cube.h"
#include "../include/TexCoords.h"

#include <cstddef>
#include <cstdint>
#include <glm/detail/qualifier.hpp>
#include <iostream>
#include <cmath>
#include <glm/geometric.hpp>
#include <sys/types.h>


Cube::Cube(float x, float y, float z, float scale, int colorIndex, int id)
	: Shape(x, y, z, scale, colorIndex, id), VAO(0), VBO(0), EBO(0) {
    shapeType = "Cube";  // Set the type as "Cube"

    // Set up OpenGL buffers
    if (setupCube() == false) std::cerr << "FAILED TO SETUP CUBE" << std::endl;
}

Cube::~Cube() {
    // Cleanup OpenGL resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

std::vector<glm::vec3> const standardPositions = {
    {-0.5f, 0.5f, -0.5f},
    {0.5f, 0.5f, -0.5f},
    {0.5f, 0.5f, 0.5f},
    {-0.5f, 0.5f, 0.5f},
    {-0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, 0.5f},
    {-0.5f, -0.5f, 0.5f},
};

std::vector<glm::vec3> const standardNormals {
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, -1.0f},
    {-1.0f, 0.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
};

glm::vec3 const standardColor  { 0.77f, 0.77f, 0.77f };

//-//////////////////////////////////////////////////////////////////
// assumes new element index every time for each plane. Yeeaaaa, but, actually.
// OK. so 
// 0, 1, 2, 3  TOP
// 0, 4, 5, 1  SIDE 1 - new data for 0 vertex. new instance of 
// Can I organize position and normal data differently s.t. normal and position data aren't part of the same element?
//       
//      to construct this, have a positions array and a normals array, then combine them. 
//      positionDataOffset
//      Cache locality?
//          -- better to have position and other data adjacent?
//          -- better to have all positions bundled, all normals bundled, etc.?            
bool Shape::constructPlane(PlaneConstructionParams params) {
    // std::cout << "CONSTRUCTING PLANE" << std::endl;

    if (params.positionIndices.size() != 4 ) {
        std::cerr << "FAILED TO SETUP CUBE. INCORRECT NUMBER OF POSITION INDICES: " << params.positionIndices.size() << std::endl;
        return false;
    }

    if (params.normalIndex < 0) {
        std::cerr << "FAILED TO SETUP CUBE. INVALID NORMAL INDEX: " << params.normalIndex << std::endl;
        return false;
    }

    if (params.normalIndex > standardNormals.size()) {
        std::cerr << "FAILED TO SETUP CUBE. normalIndex is greater than normals vector size. normalIndex: " << params.normalIndex << std::endl;
    }

    for (int k = 0; k < 4; ++k) {   
        int positionIndex = params.positionIndices.at(k);
        if (positionIndex < 0 || positionIndex >= standardPositions.size()) {
            std::cerr << "FAILED TO SETUP CUBE. positionIndices value at index " << k << " is less than 0: " << params.positionIndices[k] << std::endl;
        }

        params.vertexData.push_back(standardPositions.at(positionIndex).x);
        params.vertexData.push_back(standardPositions.at(positionIndex).y);
        params.vertexData.push_back(standardPositions.at(positionIndex).z);
        params.vertexData.push_back(standardNormals.at(params.normalIndex).x);
        params.vertexData.push_back(standardNormals.at(params.normalIndex).y);
        params.vertexData.push_back(standardNormals.at(params.normalIndex).z);
        params.vertexData.push_back(standardColor.x);
        params.vertexData.push_back(standardColor.y);
        params.vertexData.push_back(standardColor.z);
    }

    // cache faces/element indices. 
    int const topElementIndex = (params.vertexData.size() / 9) - 1;
    std::vector<std::vector<int>> const TRIS = {
        {topElementIndex - 3, topElementIndex - 2, topElementIndex - 1},
        {topElementIndex - 1, topElementIndex, topElementIndex - 3}
        };

    for (std::size_t i = 0; i < TRIS.size(); ++i) {
        faces.push_back(TRIS[i]);

        for (std::size_t j = 0; j < TRIS[i].size(); ++j) {
            params.elementData.push_back(TRIS[i][j]);
        }
    }

    return true;
}

//-//////////////////////////////////////////////////////////////////
// 
bool Cube::setupCube() {
    std::cout << "SETUP CUBE - NON-AVERAGED NORMALS " << std::endl;
    
    std::vector<float> vertexData;
    vertexData.reserve(9 * 4 * 6); // 9 floats per element * 4 elements per plane * 6 planes
    std::vector<uint> elementsData; 
    elementsData.reserve(4 * 6); // 4 elements per plane * 6 planes
    
    int const ELEMENTS_PER_LOOP = 4;
    
    for (uint i = 0 ; i < 3; ++i) {
        std::vector<int> positionIndices = {-1, -1, -1, -1};
        int normalIndex = -1;
        for (uint j = 0; j < 4; ++j) {
            // top plane
            if (i == 0 && j == 0) {
                positionIndices[0] = 0;
                positionIndices[1] = 1;
                positionIndices[2] = 2;
                positionIndices[3] = 3;
                normalIndex = 0;
                constructPlane(PlaneConstructionParams(vertexData, elementsData, positionIndices, normalIndex));
                
                break;
            } 
            
            if (i == 0) {
                std::cerr << "INVALID STATE IN CUBE CONSTRUCTION. i = " << i << ". " << "j = " << j << std::endl;
                return false;
            }

            // bottom plane
            if (i == 2) {
                positionIndices[0] = 4;
                positionIndices[1] = 5;
                positionIndices[2] = 6;
                positionIndices[3] = 7;
                normalIndex = 5;
                constructPlane(PlaneConstructionParams(vertexData, elementsData, positionIndices, normalIndex));

                break;
            }

            // if (i == 1) below

            // wait til j is greater than 0
            if (j == 0) { continue; }

            // side plane
            positionIndices[0] = j - 1;
            positionIndices[1] = i * ELEMENTS_PER_LOOP + j - 1;
            positionIndices[2] = i * ELEMENTS_PER_LOOP + j;
            positionIndices[3] = j;
            normalIndex = i + j - 1;
            constructPlane(PlaneConstructionParams(vertexData, elementsData, positionIndices, normalIndex));

            if (j < 3) { continue; }
            if (j > 3) { std::cerr << "FAILED CUBE SETUP. j = " << j << " is greater than 3. No more than 0-3 elements per loop."; }

            // wrap-around plane
            positionIndices[0] = 3;
            positionIndices[1] = 7;
            positionIndices[2] = 4;
            positionIndices[3] = 0;
            normalIndex = 4;
            constructPlane(PlaneConstructionParams(vertexData, elementsData, positionIndices, normalIndex));
        }
    }

    vertexData.shrink_to_fit();
    elementsData.shrink_to_fit();

    for (uint i = 0; i < vertexData.size(); i = i + 9) {
        vertices.push_back(glm::vec3(
            vertexData[i],
            vertexData[i+1],
            vertexData[i+2]
        ));
        normals.push_back(glm::vec3(
            vertexData[i+3],
            vertexData[i+4],
            vertexData[i+5]
        ));

    }

    // Generate an ID for one Vertex Array Object. Caches the ID in the CUBE's VAO variable.
    glGenVertexArrays(1, &VAO);
    // Bind vertex array corresponding to VAO id. This array will reflect all 
    // VBO and EBO operations until the VAO is unbound. 
    glBindVertexArray(VAO); 

    // Generate an ID for a Buffer Object. Caches the ID in the CUBE's VBO variable.
    glGenBuffers(1, &VBO);
    // Bind the Buffer Object corresponding to the cached VBO value to the GL_ARRAY_BUFFER target.
        // All operations targeting the GL_ARRAY_BUFFER will affect the Vertex Buffer Object corresponding
        // to the ID stored in the VBO variable until the ID is unbound. 
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 

    // Generates an ID for a Buffer Object. Caches the ID in the EBO variable. 
    glGenBuffers(1, &EBO);
    // Binds the GL_ELEMENT_ARRAY_BUFFER target to the buffer object that matches the ID stored in the EBO variable.
        // All operations to the GL_ELEMENT_ARRAY_BUFFER target will affect the EBO ID's matching buffer object. 
        // GL_ELEMENT_ARRAY_BUFFER operations will affect the EBO ID's matching buffer object until the ID has
            // been unbound from the GL_ELEMENT_ARRAY_BUFFER target. 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // VBO buffer operations. 
    // Copy vertexData into the VBO that is bound to GL_ARRAY_BUFFER. https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferData.xhtml
        // Targets the buffer bound to the VBO. 
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
    // 'index' is a pointer in the VAO. We are giving the pointer corresponding to that index
        // metadata for the bound VBO. Each index in a VAO points to a specific set of data. 
    // Set stride and offsets for the VBO's position, normal, and color information. 
    // Set offset to start counting index, stride, etc. to the beginning of the buffer object, hence (void*)0.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // pos
    // tell the VAO to use the VBO data that the index pointer is pointing to when making draw calls.
    glEnableVertexAttribArray(0);
    // Set the index pointer for the bound Vertex Array Object. 
    // Set offset pointer (where the first normal is found). 
    // Set size (number of floats for a normal)
    // Set type (float data type for each normal value)
    // Set normalized (values should not be normalized. They already are.)
    // Set stride (byte offset to the next set of normals in the buffer object.)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); // normal
    // Tell the VAO to use VBO data that the index pointer is pointing to. 
    glEnableVertexAttribArray(1);
    // Set the VAO's index pointer value for color values in the bound VBO.
    // Set the size (number of floats per color)
    // Set the type (color uses floats)
    // Set the normalized value (false. Don't want color values normalized).
    // Set the offset. (byte offset to where color values start in the bound VBO)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float))); // color
    glEnableVertexAttribArray(2);
    
    // EBO buffer operations. Load in element data.
        // Target - same one we bound the EBO ID matching the corresponding buffer object. 
        // This operation is what makes the buffer object an *Element* Buffer Object.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementsData.size() * sizeof(uint), elementsData.data(), GL_STATIC_DRAW);
    
    std::cout << "SETUP CUBE - COMPLETE" << std::endl;

    return true;
}

#pragma region SETUP_CUBE_WITH_AVERAGED_CUBE_NORMALS

//-//////////////////////////////////////////////////////////////////
// Sets element index data
// Computes new un-normalized normal for each vertex that is part of the plane. Sets vertex normal data.
// Sets vertex color data. 
bool Shape::constructPlaneAveragedNormals(AveragedPlaneConstructionParams params) {
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
    
    // std::cout << "ELEMENTS OUTPUT" << std::endl;
    // outputElements(params.elementData, 3);
    // std::cout << "ELEMENTS OUTPUT COMPLETE" << std::endl;
    
    //-/////////////////////////////////////////////////////
    // COMPUTE NORMALS. ADD COLORS.
    //-/////////////////////////////////////////////////////
    
    for (uint32_t p = 0; p < 4; ++p) {
        planeVertPositions[p] = glm::vec3(  params.vertexData[elementIndices[p] * params.VALUES_PER_VERTEX    ], 
                                            params.vertexData[elementIndices[p] * params.VALUES_PER_VERTEX + 1],
                                            params.vertexData[elementIndices[p] * params.VALUES_PER_VERTEX + 2]      );
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
    
    // add more vertices and normals. Do not procedurally calculate normals using an average. leads to bad juju. 

    for(uint32_t p = 0; p < 4; ++p) {
        glm::vec3 normal (  params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERTEX    ],
                            params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERTEX + 1],
                            params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERTEX + 2]   );
        // Add cross product with current normal to get new normal.
        normal += planeNormal;

        // Set normal to equal new normal value
        params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERTEX] = normal.x;
        params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERTEX + 1] = normal.y;
        params.vertexData[3 + elementIndices[p] * params.VALUES_PER_VERTEX + 2] = normal.z;

        // Set color values
        float const newColor = 0.77f;
        params.vertexData[6 + elementIndices[p] * params.VALUES_PER_VERTEX    ] = newColor;
        params.vertexData[6 + elementIndices[p] * params.VALUES_PER_VERTEX + 1] = newColor;
        params.vertexData[6 + elementIndices[p] * params.VALUES_PER_VERTEX + 2] = newColor;
    }

    // outputVertices(params.vertexData, params.VALUES_PER_VERT);

    std::cout << "COMPLETED PLANE CONSTRUCTION" << std::endl;


    return true;
}

//-//////////////////////////////////////////////////////////////
//
bool Cube::setupCubeAveragedNormals() {

    std::cout << "SETTING-UP CUBE" << std::endl;        
    normalConfig = NormalConfig::SHARED;
    
    uint32_t const FLOATS_PER_ELEMENT = 9; // 3 pos + 3 norm + 3 col
    uint32_t const ELEMENTS_PER_LOOP = 4;
    uint32_t const MAX_LOOPS = 2;
    
    std::vector<float> vertexData;
    vertexData.reserve(8 * FLOATS_PER_ELEMENT); // 8 vertices per cube

    std::vector<unsigned int> elementData;
    elementData.reserve(36); // 6 sides * 2 triangles per side * 3 verts per triangle

    for(uint32_t i = 0; i < MAX_LOOPS; ++i) {
        std::cout << "LOOP " << i << std::endl;

        float const LOOP_HEIGHT = 0.5*scale - (static_cast<float>(i) / (MAX_LOOPS * 0.5)) * scale;
        uint32_t const FULL_LOOP_OFFSET = i * ELEMENTS_PER_LOOP * FLOATS_PER_ELEMENT;
        
        for (uint32_t j = 0; j < ELEMENTS_PER_LOOP; ++j) {
            std::cout << "ELEMENT THIS LOOP: " << j << std::endl;

            //-//////////////////////////////////////////////////////////////
            // CREATE VERTS. 
            //-//////////////////////////////////////////////////////////////

            if (j == 0) { // first ELEMENT of new loop
                std::cout << "FIRST ELEMENT OF NEW LOOP: " << i * ELEMENTS_PER_LOOP << std::endl;
                float const x = 0.5 * -scale;
                float const y = LOOP_HEIGHT;
                float const z = 0.5 * -scale;
                vertexData.push_back(x);
                vertexData.push_back(y);
                vertexData.push_back(z);

                vertices.push_back(glm::vec3(x, y , z));

                // assign dummy values to normal and color. These will be changed later. 
                for (uint8_t k = 0; k < FLOATS_PER_ELEMENT - 3; ++k) {
                    vertexData.push_back(0.0f);
                }
                // outputVertices(vertexData, VALUES_PER_VERT);

                continue; // need more verts to make a plane.
            }
            
            uint32_t const LAST_VERTEX_INDEX = FULL_LOOP_OFFSET + FLOATS_PER_ELEMENT * (j - 1);
            uint32_t const THIS_VERTEX_INDEX = FULL_LOOP_OFFSET + FLOATS_PER_ELEMENT * j;

            std::cout << "create next vertex" << j << std::endl;
            std::cout << "LAST_VERTEX_INDEX: " << LAST_VERTEX_INDEX << std::endl;
            std::cout << "THIS_VERTEX_INDEX: " << THIS_VERTEX_INDEX << std::endl;

            // Create next vertex
            {
                double trigInputForOffset = 2 * ShapeMath::PI() * (static_cast<double>(j - 1) / static_cast<double>(ELEMENTS_PER_LOOP));

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
                // assign normal and color to 0f. They are updated during plane construction.
                for (uint8_t k = 0; k < FLOATS_PER_ELEMENT - 3; ++k) {
                    vertexData.push_back(0.0f);
                }

                // outputVertices(vertexData, VALUES_PER_VERT);
            }

            //-//////////////////////////////////////////////////////////////
            // CONSTRUCT PLANES. 
            //-//////////////////////////////////////////////////////////////
            
            std::vector<PlaneType> planeTypes;
            planeTypes.reserve(3); // max of 3 planes constructed per cube

            // Always add a side plane when i > 0 and j > 0. 
            if (i > 0) { planeTypes.push_back(PlaneType::INTERMEDIATE); }
            
            if (j == ELEMENTS_PER_LOOP - 1) {
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
                constructPlaneAveragedNormals(AveragedPlaneConstructionParams(planeTypes[p], MAX_LOOPS, ELEMENTS_PER_LOOP, FLOATS_PER_ELEMENT, i, j, elementData, vertexData));
            }
        }

        outputVertices(vertexData, FLOATS_PER_ELEMENT);
    }

    // normalize normals.
    // the plane construction algorithm sums a new plane's normal into the old normal value. We normalize at the
    //      end to get correct values we square_root(x^2 + y^2 + z^2);
    for (uint32_t i = 0; i < MAX_LOOPS * ELEMENTS_PER_LOOP; ++i) {
        glm::vec3 normal (vertexData[3 + i * FLOATS_PER_ELEMENT],
                        vertexData[3 + i * FLOATS_PER_ELEMENT + 1],
                        vertexData[3 + i * FLOATS_PER_ELEMENT + 2]);
        normal = normalize(normal);
        vertexData[3 + i * FLOATS_PER_ELEMENT] = normal.x;
        vertexData[3 + i * FLOATS_PER_ELEMENT + 1] = normal.y;
        vertexData[3 + i * FLOATS_PER_ELEMENT + 2] = normal.z;

        normals.reserve(vertexData.size() / 3);
        normals.push_back(normal);
    }

    vertexData.shrink_to_fit();
    outputVertices(vertexData, FLOATS_PER_ELEMENT);

    elementData.shrink_to_fit();
    outputElements(elementData, 3);

    normalConfig = NormalConfig::SHARED;

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_ELEMENT * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_ELEMENT * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_ELEMENT * sizeof(float), (void*)(6 * sizeof(float))); // Color
    glEnableVertexAttribArray(2);

    // UVs go in their own buffer on attribute 3, while the VAO is still bound.
    // uploadTexCoords();

    glBindVertexArray(0); // Unbind VAO

    return true;
}

#pragma endregion

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
    glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Disable lighting after drawing the cube (for axis rendering)
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 0);
    }
}

