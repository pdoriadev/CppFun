// Wavefront OBJ parsing -- see include/ObjParser.h for why this is its own
// translation unit. Student work; everything around it is given.

#include "../include/ObjParser.h"

#include <cstdlib>
#include <sstream>
#include <iostream>

namespace {

// Pulls the three fields out of one "a/b/c" corner. Any of b and c may be
// absent: "a", "a/b", "a//c" and "a/b/c" are all legal OBJ.
//
// Indices are returned already converted to zero-based, or -1 when the field
// was not present. OBJ counts from 1, and OBJ also allows NEGATIVE indices
// meaning "counting back from the end of the list so far", which is why the
// caller passes the current counts in.
void parseCorner(const std::string& field,
                 size_t vCount, size_t vtCount, size_t vnCount,
                 int& vi, int& ti, int& ni) {
    vi = ti = ni = -1;

    const size_t s1 = field.find('/');
    const size_t s2 = (s1 == std::string::npos) ? std::string::npos
                                                : field.find('/', s1 + 1);

    const std::string a = field.substr(0, s1);
    const std::string b = (s1 == std::string::npos) ? std::string()
                        : field.substr(s1 + 1,
                              (s2 == std::string::npos) ? std::string::npos
                                                        : s2 - s1 - 1);
    const std::string c = (s2 == std::string::npos) ? std::string()
                                                    : field.substr(s2 + 1);

    struct Resolve {
        static int one(const std::string& text, size_t count) {
            if (text.empty()) return -1;
            const long raw = std::atol(text.c_str());
            if (raw > 0)  return static_cast<int>(raw - 1);
            if (raw < 0)  return static_cast<int>(static_cast<long>(count) + raw);
            return -1;    // index 0 is not legal in OBJ
        }
    };

    vi = Resolve::one(a, vCount);
    ti = Resolve::one(b, vtCount);
    ni = Resolve::one(c, vnCount);
}

} // namespace

MeshDataType const convertMeshDataTypeStringToEnum(std::string const type) {
    int static const INT_TYPES[static_cast<size_t>(MeshDataType::COUNT)] = {
        std::stoi("v"),     // POSITION_INT_STRING 
        std::stoi("vn"),    // NORMAL_INT_STRING 
        std::stoi("vt"),    // TEXTURE_INT_STRING
        std::stoi("f")      // FACE_INT_STRING 
    };
    
    int const TYPE_AS_INT = std::stoi(type);
    for (int i = 0; i < (uint)MeshDataType::COUNT; ++i) {
        if (TYPE_AS_INT == INT_TYPES[i]) { return (MeshDataType)i; }
    }

    return MeshDataType::INVALID;
}

bool parseObj(std::istream& in, ObjMesh& out) {
    // TODO(objparser): parse v, vn, vt and f lines into the ObjMesh
    // Read the stream a line at a time and dispatch on the first token.
    // f corners are "a/b/c" with b and c each optional; indices are
    // 1-based and may be negative (counting back from the end).
    // Resolve each vt index to a coordinate here and push one per corner,
    // so faces keep their stride of 2 (vertex, normal).
    
    // Ignoring vt for now (assuming that's texture data)

    std::cout << "Parsing obj file" << std::endl;

    uint16_t MAX_BUFFER_SIZE = 1024;
    char lineBuffer[MAX_BUFFER_SIZE];
    while(in.getline(lineBuffer, MAX_BUFFER_SIZE)) {
        std::stringstream lineStream(lineBuffer);
        std::string dataTypeStr;
        lineStream >> dataTypeStr;
        MeshDataType const MESH_DATA_TYPE = convertMeshDataTypeStringToEnum(dataTypeStr);

        size_t vCount = 0;
        size_t vtCount = 0;
        size_t vnCount = 0;

        // assign data
        switch(MESH_DATA_TYPE) {
            /* vec3 cases */ {
            glm::vec3 vec3Data;
            case MeshDataType::POSITION:
                lineStream >> vec3Data[0] >> vec3Data[1] >> vec3Data[2];
                out.vertices.push_back(vec3Data);
                ++vCount;
                break;
            case MeshDataType::NORMAL:
                lineStream >> vec3Data[0] >> vec3Data[1] >> vec3Data[2];
                out.normals.push_back(vec3Data);
                ++vnCount;
                break;
            case MeshDataType::TEXTURE:
                // not implemented yet. move to next line.
                continue;
            } // End of vec3 cases
            case MeshDataType::FACE: {
                // vectors to cache in faces vector<vector<int>>
                std::vector<int> posVertIndices {0, 0, 0};
                std::vector<int> normalVertIndices {0, 0, 0};
                int dummyTexureIndex = 0;

                std::string cornerStr;
                for (uint i = 0; i < 3; ++i) {
                    lineStream >> cornerStr;
                    parseCorner(cornerStr, vCount, vtCount, vnCount, posVertIndices[i], dummyTexureIndex, normalVertIndices[i]);
                }


                // I need to parse each 'corner' when I reach a face. I'll have already gotten the line, right?
                // yes. I have the linestream. NOW, each case handles that line stream differently. The face has to 
                // parse each face 'corner' or vertex. each corner has 1-3 indices. Always at least one position index. 
                // Keep track of the count. 
                //      Is 'count' how many faces have been counted? How many vertex indices?
                // 

                // OLD APPROACH
                // caching stream into string for easy parsing around ' ' and '/' characters.            
                
                // std::string const numbers = "0123456789";
                // uint8_t i = 0; // i'th number in string. 
                // std::size_t numStart = lineStr.find_first_of(numbers.c_str());
                // while (numStart != std::string::npos) {
                //     // covers ' ' and '/' cases. 
                //     std::size_t firstNotNum = lineStr.find_first_not_of(numbers.c_str(), numStart+1);

                //     int number = std::stoi(lineStr.substr(numStart, firstNotNum - 1)) - 1;
                //     switch(i) {
                //         case 0: // pos a. 1st vert.
                //             posVertIndices[0] = number;
                //             break;
                //         case 1: break; // ignore b
                //         case 2: // normal c. 1st vert.
                //             normalVertIndices[0] = number;
                //             break;
                //         case 3: // pos d. 2nd vert.
                //             posVertIndices[1] = number;
                //             break;
                //         case 4: break; // igonre e
                //         case 5: // normal f. 2nd vert. 
                //             normalVertIndices[1] = number;
                //             break;
                //         case 6: // pos g. 3rd vert. 
                //             posVertIndices[2] = number;
                //             break;
                //         case 7: break; // h. ignore. 
                //         case 8: // normal i. 3rd vert.
                //             normalVertIndices[2] = number;
                //             break;
                //         default:
                //             std::cout << "ERROR - More than 8 numbers found in faces line." << std::endl;
                //             break;
                //     }

                //     // setup next loop. start from position after last ' ' or '/'
                //     numStart = lineStr.find_first_of(numbers.c_str(), firstNotNum + 1);
                //}

                out.faces.push_back(posVertIndices);
                out.faces.push_back(normalVertIndices); // how does this work though???
                break;
            } // End of FACE case.
            default:
                // invalid type. move to next line.
                std::cout << "ERROR - INVALID MESH DATA: " << (int)MESH_DATA_TYPE << std::endl;
                continue;
        }     
    }
    
    std::cout << "finished parsing obj file" << std::endl;

    return true;
}
