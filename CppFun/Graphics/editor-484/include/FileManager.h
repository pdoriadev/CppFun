#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>
#include "ShapeManager.h"
#include "json.hpp" // Include JSON library (nlohmann/json)

using json = nlohmann::json;

class FileManager {
public:
    // Save all shapes in the scene to a JSON file
    static bool saveScene(const std::string& filename, const ShapeManager& shapeManager);

    // Load all shapes from a JSON file and reconstruct them
    static bool loadScene(const std::string& filename, ShapeManager& shapeManager);
};

#endif // FILE_MANAGER_H

