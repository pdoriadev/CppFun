#ifndef FILEIMPORTER_H
#define FILEIMPORTER_H

#ifndef MAX_PATH
#define MAX_PATH 260 // Define MAX_PATH manually if not already defined
#endif

#include "ShapeManager.h"
#include "Curve.h"
#include "Joint.h"
#include "MatrixStack.h"
#include "ImportShape.h"
#include "ImportCurve.h"
#include "ImportCharacter.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tinyfiledialogs.h>

#include <unistd.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cctype>  // For std::toupper and std::tolower

// The FileImporter class encapsulates the logic for importing .obj, .swp, and character files.

class FileImporter {
public:

    // Read in dim-dimensional control points into a vector
    std::vector<glm::vec3> readCps(std::istream &file, unsigned dim);

    // Retrieve location of executable
    std::string getExecutableDirectory();
    
    // Imports the .obj file and adds a new shape to the scene
    // When set, the next import uses this path instead of opening a file
    // dialog. Consumed by the import, so it does not leak into the next one.
    // This is how scene loading re-imports meshes without user interaction.
    void setPendingPath(const std::string& p) { pendingPath = p; }

    int importObjFile(ShapeManager& shapeManager);
    int importSwpFile(ShapeManager& shapeManager);
    int importCharacterFile(ShapeManager& shapeManager);

private:

    // Extracts the shape type from the file name
    std::string extractShapeType(const std::string& filename);
	
private:
    std::string pendingPath;
};

#endif // FILEIMPORTER_H
