#include "ObjParser.h"
#include "FileImporter.h"

// Read control points from a file
std::vector<glm::vec3> FileImporter::readCps(std::istream &file, unsigned dim) {    
   
    // Number of control points    
    unsigned n;
    file >> n;

    // vector of control points
    std::vector<glm::vec3> cps;

    for (unsigned i = 0; i < n; ++i) {
    
        char delim;
        float x, y, z;

        switch (dim) {
            case 2:
                file >> delim >> x >> y;
                cps.push_back(glm::vec3(x, y, 0));
                file >> delim;
                break;
            case 3:
                file >> delim >> x >> y >> z;
                cps.push_back(glm::vec3(x, y, z));
                file >> delim;
                break;            
            default:
                std::cerr << "Error: Unsupported dimension: " << dim << std::endl;
                abort();
        }
    }
        
    return cps;
}


std::string FileImporter::getExecutableDirectory() {
    char buffer[MAX_PATH];
    if (getcwd(buffer, sizeof(buffer))) {
        return std::string(buffer);
    }
    return "";
}

// Function to import a selected obj file
int FileImporter::importObjFile(ShapeManager& shapeManager) {

    std::string execDir = getExecutableDirectory();
    
    if (!execDir.empty()) {

        // Assuming you want to set the default path to a "data" folder relative to the executable
        std::string defaultPath = execDir + "/data/obj/*.obj";  // Linux/Mac
        // std::string defaultPath = execDir + "\\data";        // Windows

	// Filter for obj files
	const char* fileFilters[] = {"*.obj"};
	
        // Open file chooser dialog for selecting an obj file	
        const char* selectedFile = !pendingPath.empty()
            ? pendingPath.c_str()
            : tinyfd_openFileDialog(
            "Select an OBJ file",
            defaultPath.c_str(),  // Default relative path
            1,                    // Number of filters
            fileFilters,          // Filters array
            "OBJ Files (*.obj)",  // Filter description
            0                     // Allow multiple selection (0 = no, 1 = yes)
        );

	if (!selectedFile) {
	    std::cerr << "File selection cancelled or failed." << std::endl;
	    return 0;
        }

	// Get the shape type from the filename without the extension
        std::string newShapeType = extractShapeType(selectedFile);
        
        newShapeType[0] = std::toupper(newShapeType[0]);
        for (size_t i = 1; i < newShapeType.length(); ++i) {
            newShapeType[i] = std::tolower(newShapeType[i]);
        }
							
        std::ifstream file(selectedFile);
        if (!file.is_open()) {
            std::cerr << "Unable to open file: " << selectedFile << std::endl;
            return 0;
        }

        // The parse itself lives in ObjParser.cpp so it can be unit tested
        // without a dialog or a GL context, and so a solution library can
        // replace it as a whole translation unit. See include/ObjParser.h.
        ObjMesh mesh;
        const bool parsed = parseObj(file, mesh);
        file.close();

        if (!parsed) {
            std::cerr << "No usable geometry in: " << selectedFile << std::endl;
            return 0;
        }

	// Create the new ImportShape and populate it
	ImportShape* newShape = new ImportShape(0.0f, 0.0f, 0.0f, 1.0f, 1, shapeManager.incrementShapeCounter());
	newShape->setVertices(mesh.vertices);
	newShape->setNormals(mesh.normals);
	newShape->setFaces(mesh.faces);
	// Only attach UVs when the file actually carried vt lines. A mesh with no
	// parameterisation must leave texCoords empty, so attribute 3 stays
	// disabled and hasTexture stays false -- untextured rendering is then
	// bit-for-bit what it was before texturing existed.
	if (mesh.hasTexCoords && mesh.texCoords.size() == mesh.faces.size() * 3) {
	    newShape->setTexCoords(mesh.texCoords);
	}
	newShape->setupShape();
	
	// Add importedShape to your list of shapes or render it directly
	shapeManager.addShape(newShape);
	shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
	shapeManager.getSelectedShape()->setShapeType(newShapeType);
	shapeManager.getSelectedShape()->setSourcePath(selectedFile);
	pendingPath.clear();

    }
  
    return 1;
    
}

// Function to import a selected obj file
int FileImporter::importSwpFile(ShapeManager& shapeManager) {

    std::string execDir = getExecutableDirectory();
    if (!execDir.empty()) {

        // Assuming you want to set the default path to a "data" folder relative to the executable
        std::string defaultPath = execDir + "/data/swp/*.swp";  // Linux/Mac
        // std::string defaultPath = execDir + "\\swp";         // Windows

	// Filter for swp files
	const char* fileFilters[] = {"*.swp"};
	
	// Open file chooser dialog for selecting an obj file	
        const char* selectedFile = !pendingPath.empty()
            ? pendingPath.c_str()
            : tinyfd_openFileDialog(
            "Select an SWP file",
            defaultPath.c_str(),  // Default relative path
            1,                    // Number of filters
            fileFilters,          // Filters array
            "SWP Files (*.swp)",  // Filter description
            0                     // Allow multiple selection (0 = no, 1 = yes)
        );

        if (!selectedFile) {
            std::cerr << "File selection canceled or failed." << std::endl;
            return 0;
        }

	// Get the shape type from the filename without the extension
	std::string newShapeType = extractShapeType(selectedFile);

	// The casts are not cosmetic: toupper/tolower take an int that must be
	// representable as unsigned char (or EOF). Passing a plain char is
	// undefined behaviour the moment a filename contains a byte above 127.
	newShapeType[0] = static_cast<char>(
	    std::toupper(static_cast<unsigned char>(newShapeType[0])));
	for (size_t i = 1; i < newShapeType.length(); ++i) {
            newShapeType[i] = static_cast<char>(
                std::tolower(static_cast<unsigned char>(newShapeType[i])));
	}
		
        std::ifstream file(selectedFile);
        if (!file.is_open()) {
            std::cerr << "Unable to open file: " << selectedFile << std::endl;
            return 0;
        }

        ImportCurve* importCurve = new ImportCurve(0.0f, 0.0f, 0.0f, 1.0f, 30, shapeManager.incrementShapeCounter());
        // Remember where this came from so a saved scene can re-import it.

        std::string objType, objName;
        unsigned steps;
        float radius; // Variable to store radius for circle
		
        while (file >> objType >> objName) {

            if (objType == "bez2" || objType == "bsp2" || objType == "bez3" || objType == "bsp3") {
            
                file >> steps;
                
		// Create the new curve
		Curve curve(objName, steps, objType);

                // Determine the dimension based on the curve type
                unsigned dim = (objType == "bez2" || objType == "bsp2") ? 2 : 3;

                // Pass the correct dimension to readCps
                std::vector<glm::vec3> controlPoints = readCps(file, dim);

		// Read the controlpoints into the curve		
                curve.setControlPoints(controlPoints);
                
		// Evaluate the controlpoints to vertices
		CurvePoints newCurvePoints = (objType == "bez2" || objType == "bez3") ? 
                    Curve::evalBezier(controlPoints, steps) : Curve::evalBspline(controlPoints, steps);
                curve.setCurvePoints(newCurvePoints);
                
                // Add the curve to the ImportCurve object                
                importCurve->addCurve(curve);
                
            } else if (objType == "circ") {
            
                // Read the radius
                file >> steps >> radius;
                
               // Create a new Curve object for the circle
                Curve circleCurve(objName, steps, objType);

                // Generate circle points using Curve::evalCircle
                CurvePoints circlePoints = Curve::evalCircle(radius, steps);
                circleCurve.setCurvePoints(circlePoints);

                // Add the circle to the ImportCurve object
                importCurve->addCurve(circleCurve);
                
            } else if (objType == "srev") {

                // Read the curve name
                file >> steps >> objName;

                // Find the profile curve by name and add to the ImportCurve object
                for (const auto& curve : importCurve->getCurves()) {
                    if (curve.getName() == objName) {
                        importCurve->addSurface(Surface::makeSurfRev(curve, steps));
                    }
                }
                
            } else if (objType == "gcyl") {

                // Read the profile and sweep curve names
                std::string profName, sweepName;
                file >> profName >> sweepName;

                // Find the profile and sweep curves by name
                const Curve* profile = nullptr;
                const Curve* sweep = nullptr;
                for (const auto& curve : importCurve->getCurves()) {
                    if (curve.getName() == profName) profile = &curve;
                    if (curve.getName() == sweepName) sweep = &curve;
                }

                // Find the profile and sweep curves by name and add to the ImportCurve object
                if (profile && sweep) {
                    Surface surface = Surface::makeGenCyl(*profile, *sweep);
                    importCurve->addSurface(surface);
                } else {
                    std::cerr << "Error: Could not find matching profile or sweep curve for gcyl.\n";
                }
                
            }        

        }

        file.close();
        
        importCurve->setupCurveBuffer();
        importCurve->setupSurfaceBuffer();

        // Add the ImportCurve object to ShapeManager
        shapeManager.addShape(importCurve);

	// Add importedShape to your list of shapes or render it directly
	shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
	shapeManager.getSelectedShape()->setShapeType(newShapeType);
	
	// For debug
        // importCurve->printControlPointsAndCurvePoints();

    }
    return 1;
}


// Function to import a selected obj file
int FileImporter::importCharacterFile(ShapeManager& shapeManager) {

    std::string execDir = getExecutableDirectory();
    if (!execDir.empty()) {

        // Assuming you want to set the default path to a "data" folder relative to the executable
        std::string defaultPath = execDir + "/data/characters/*.obj";  // Linux/Mac 
        // std::string defaultPath = execDir + "\\characters";         // Windows

	// Filter for obj files
	const char* fileFilters[] = {"*.obj"};
	
	// Open file chooser dialog for selecting an obj file	
        const char* selectedFile = !pendingPath.empty()
            ? pendingPath.c_str()
            : tinyfd_openFileDialog(
            "Select an OBJ file",
            defaultPath.c_str(),  // Default relative path
            1,                    // Number of filters
            fileFilters,          // Filters array
            "OBJ Files (*.swp)",  // Filter description
            0                     // Allow multiple selection (0 = no, 1 = yes)
        );

        if (!selectedFile) {
            std::cerr << "File selection canceled or failed." << std::endl;
            return 0;
        }

	// Get the shape type from the filename without the extension
	std::string newShapeType = extractShapeType(selectedFile);

	// See the note on the same loop in importSwpFile.
	newShapeType[0] = static_cast<char>(
	    std::toupper(static_cast<unsigned char>(newShapeType[0])));
	for (size_t i = 1; i < newShapeType.length(); ++i) {
		newShapeType[i] = static_cast<char>(
		    std::tolower(static_cast<unsigned char>(newShapeType[i])));
	}
							
        std::vector<glm::vec3> vertices;
	std::vector<std::vector<int>> faces;
			
	std::ifstream file(selectedFile);
	if (!file.is_open()) {
		std::cerr << "Unable to open file: " << selectedFile << std::endl;
		return 0;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream ss(line);
		std::string prefix;
		std::vector<int> face;  // Store vertex and normal indices
			
		ss >> prefix;

		if (prefix == "v") {
			float x, y, z;
			ss >> x >> y >> z;
                        vertices.push_back(glm::vec3(x, y, z));
		} else if (prefix == "f") {
			int fx, fy, fz;
			ss >> fx >> fy >> fz;
			face.push_back(fx - 1);
			face.push_back(fy - 1);
			face.push_back(fz - 1);					
			faces.push_back(face);
		}
	}
	file.close();
			
	std::string selectedFileSkel = selectedFile;			
	std::size_t pos = selectedFileSkel.find_last_of('.');

	// Check if an extension exists; if so, replace it
	if (pos != std::string::npos) {
		selectedFileSkel = selectedFileSkel.substr(0, pos) + ".skel";
	} else {
		selectedFileSkel = selectedFileSkel + ".skel";				
	}

	Joint* rootJoint;
	std::vector<Joint*> joints;
	std::vector<int> parentIndices;

	std::ifstream fileSkel(selectedFileSkel);
	if (!fileSkel.is_open()) {
		std::cerr << "Unable to open .skel file: " << selectedFileSkel << std::endl;
		return 0;
	} else {

				std::string lineSkel;

								
				while(getline(fileSkel, lineSkel)){
	
					float x, y, z;
					int parentIndex;
					

					std::istringstream skeletonString;
					skeletonString.str(lineSkel);
					skeletonString >> x >> y >> z >> parentIndex;

					Joint *joint = new Joint;
                                       joint->setTransform(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)));

					joints.push_back(joint);
					parentIndices.push_back(parentIndex);

				}


				for (size_t i = 1; i < joints.size(); ++i) {
					int parentIndex = parentIndices[i];
					joints[parentIndex]->addChild(joints[i]);
				}

				rootJoint = joints.front();

			}

			fileSkel.close();

			
			std::vector< std::vector< float > > attachments;

			
			std::string selectedFileAttach = selectedFile;			
			pos = selectedFileAttach.find_last_of('.');

			// Check if an extension exists; if so, replace it
			if (pos != std::string::npos) {
				selectedFileAttach = selectedFileAttach.substr(0, pos) + ".attach";
			} else {
				selectedFileAttach = selectedFileAttach + ".attach";				
			}

			std::ifstream fileAttach(selectedFileAttach);
			if (!fileAttach.is_open()) {
				std::cerr << "Unable to open .skel file: " << selectedFileAttach << std::endl;
				return 0;
			} else {

				// Read the attachment weights

				std::string lineAttach;
								
				while(getline(fileAttach, lineAttach)){

					std::istringstream attachString;

					float val;
					attachString.str(lineAttach);
					std::vector<float> attachList;

					while(attachString >> val){

						attachList.push_back(val);

					}

					attachments.push_back(attachList);

				}

			}
			
			// Get the shape type from the filename without the extension
			
			ImportCharacter* importCharacter = new ImportCharacter(0.0f, 0.0f, 0.0f, 1.0f, 12, shapeManager.incrementShapeCounter());
        // Remember where this came from so a saved scene can re-import it.

			importCharacter->setVertices(vertices);
			importCharacter->setFaces(faces);
			importCharacter->calculateNormals();
			importCharacter->setBindVertices(vertices);
			importCharacter->getSkeletalModel().setRootJoint(rootJoint);
			importCharacter->getSkeletalModel().setJoints(joints);
			importCharacter->setAttachments(attachments);

			importCharacter->getSkeletalModel().computeBindWorldToJointTransforms();
			importCharacter->getSkeletalModel().updateCurrentJointToWorldTransforms();

                        importCharacter->setupMeshBuffer();
                        importCharacter->setupJointBuffer();
                        importCharacter->setupBoneBuffer();

			// Add the ImportCharacter object to ShapeManager
			shapeManager.addShape(importCharacter);	

			// Add ImportCharacter to your list of shapes or render it directly

			shapeManager.setSelectedShapeByLastAdded();  // Select the last shape added
			shapeManager.getSelectedShape()->setShapeType(newShapeType); 
			


		}
	

	return 1;

}




// Extract shape type from file name
std::string FileImporter::extractShapeType(const std::string& filename) {
    size_t lastSlash = filename.find_last_of("/\\");
    size_t lastDot = filename.find_last_of('.');

    if (lastSlash == std::string::npos) {
        lastSlash = 0;
    } else {
        lastSlash += 1;
    }

    if (lastDot == std::string::npos || lastDot < lastSlash) {
        lastDot = filename.size();
    }

    return filename.substr(lastSlash, lastDot - lastSlash);
}

