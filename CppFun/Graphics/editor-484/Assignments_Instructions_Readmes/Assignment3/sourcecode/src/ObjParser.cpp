// Wavefront OBJ parsing -- see include/ObjParser.h for why this is its own
// translation unit. Student work; everything around it is given.

#include "../include/ObjParser.h"

#include <cstddef>
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
void parseCorner(std::string const& field,
                 size_t const vCount, size_t const vtCount, size_t const vnCount,
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
        static int one(const std::string& text, size_t const count) {
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

namespace ObjMeshData {
    int const getStringTypeIndex(std::string const STR_TYPE) {
        for (std::size_t i = 0; i < stringTypes.size(); ++i) {
            if (STR_TYPE == stringTypes[i]) return i;
        }
        return -1;
    }

    MeshDataType const convertStringTypeToEnumType(std::string const STR_TYPE) {
        int const index = getStringTypeIndex(STR_TYPE);
        if (index < 0) return MeshDataType::INVALID;
        return meshDataTypes[index];     
    }
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

    uint16_t MAX_BUFFER_SIZE = 256;
    char lineBuffer[MAX_BUFFER_SIZE];
    while(in.getline(lineBuffer, MAX_BUFFER_SIZE)) {
        std::stringstream lineStream(lineBuffer);
        std::string dataTypeStr;
        lineStream >> dataTypeStr;
        ObjMeshData::MeshDataType const MESH_DATA_TYPE = ObjMeshData::convertStringTypeToEnumType(dataTypeStr);

        size_t vpCount = 0;
        size_t vtCount = 0;
        size_t vnCount = 0;

        // assign data
        switch(MESH_DATA_TYPE) {
            /* vec3 cases */ {
            glm::vec3 vec3Data;
            case ObjMeshData::MeshDataType::POSITION:
                lineStream >> vec3Data[0] >> vec3Data[1] >> vec3Data[2];
                out.vertices.push_back(vec3Data);
                ++vpCount;
                break;
            case ObjMeshData::MeshDataType::NORMAL:
                lineStream >> vec3Data[0] >> vec3Data[1] >> vec3Data[2];
                out.normals.push_back(vec3Data);
                ++vnCount;
                break;
            } // End of vec3 cases
            case ObjMeshData::MeshDataType::TEXTURE:
                // not implemented yet. move to next line.
                continue;
            case ObjMeshData::MeshDataType::FACE: {
                // vectors to cache in faces vector<vector<int>>
                std::vector<int> dataIndices {0, 0, 0, 0, 0, 0};
                int dummyTexureIndex = 0;
                std::string cornerStr;
                
                for (uint i = 0; i < 3; ++i) {
                    lineStream >> cornerStr;
                    parseCorner(cornerStr, vpCount, vtCount, vnCount, dataIndices[i], dummyTexureIndex, dataIndices[i+3]);
                }

                out.faces.push_back(dataIndices);
                break;
            } // End of FACE case.
            default:
                // invalid type. move to next line.
                // std::cout << "ERROR - INVALID MESH DATA. String: " << dataTypeStr << ". Enum: " << (int)MESH_DATA_TYPE << std::endl;
                continue;
        }     
    }
    
    std::cout << "finished parsing obj file" << std::endl;

    return true;
}
