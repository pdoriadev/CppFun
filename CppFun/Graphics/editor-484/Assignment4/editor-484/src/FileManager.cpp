#include "FileManager.h"

void FileManager::saveScene(const ShapeManager& shapeManager) {
    Json::Value root;
    
    for (const auto& shape : shapeManager.getShapes()) {
        Json::Value shapeJson;
        shapeJson["type"] = shape->getShapeType();
        shapeJson["position"] = Json::arrayValue;
        shapeJson["position"].append(shape->getX());
        shapeJson["position"].append(shape->getY());
        shapeJson["position"].append(shape->getZ());
        
        shapeJson["rotation"] = Json::arrayValue;
        shapeJson["rotation"].append(shape->getAngleX());
        shapeJson["rotation"].append(shape->getAngleY());
        shapeJson["rotation"].append(shape->getAngleZ());
        
        shapeJson["scale"] = Json::arrayValue;
        shapeJson["scale"].append(shape->getScale());
        shapeJson["nonUniformScale"] = Json::arrayValue;
        auto nonUniformScale = shape->getNonUniformScale();
        shapeJson["nonUniformScale"].append(nonUniformScale.x);
        shapeJson["nonUniformScale"].append(nonUniformScale.y);
        shapeJson["nonUniformScale"].append(nonUniformScale.z);
        
        shapeJson["useUniformScaling"] = shape->isUsingUniformScaling();
        
        root["shapes"].append(shapeJson);
    }
    
    char const* savePath = tinyfd_saveFileDialog("Save Scene", "scene.json", 0, NULL, "JSON files");
    if (savePath) {
        std::ofstream file(savePath);
        file << root;
    }
}

void FileManager::loadScene(ShapeManager& shapeManager) {
    char const* loadPath = tinyfd_openFileDialog("Load Scene", "", 0, NULL, "JSON files", 0);
    if (!loadPath) return;
    
    std::ifstream file(loadPath);
    Json::Value root;
    file >> root;
    
    shapeManager.clearShapes();
    
    for (const auto& shapeJson : root["shapes"]) {
        std::string type = shapeJson["type"].asString();
        float x = shapeJson["position"][0].asFloat();
        float y = shapeJson["position"][1].asFloat();
        float z = shapeJson["position"][2].asFloat();
        
        float angleX = shapeJson["rotation"][0].asFloat();
        float angleY = shapeJson["rotation"][1].asFloat();
        float angleZ = shapeJson["rotation"][2].asFloat();
        
        float scale = shapeJson["scale"].asFloat();
        glm::vec3 nonUniformScale(shapeJson["nonUniformScale"][0].asFloat(),
                                  shapeJson["nonUniformScale"][1].asFloat(),
                                  shapeJson["nonUniformScale"][2].asFloat());
        
        bool useUniformScaling = shapeJson["useUniformScaling"].asBool();
        
        Shape* shape = nullptr;
        if (type == "Cube") {
            shape = new Cube(x, y, z, scale, 2, shapeManager.incrementShapeCounter());
        } else if (type == "Sphere") {
            shape = new Sphere(x, y, z, scale, 2, shapeManager.incrementShapeCounter());
        } else if (type == "Pyramid") {
            shape = new Pyramid(x, y, z, scale, 2, shapeManager.incrementShapeCounter());
        } else if (type == "Teapot") {
            shape = new Teapot(x, y, z, scale, 4, shapeManager.incrementShapeCounter());
        } else if (type == "Icosahedron") {
            shape = new Icosahedron(x, y, z, scale, 2, shapeManager.incrementShapeCounter());
        } else if (type == "Custom") {
            shape = new Custom(x, y, z, scale, 11, shapeManager.incrementShapeCounter());
        } else if (type == "ImportShape") {
            shape = new ImportShape(x, y, z, scale, 1, shapeManager.incrementShapeCounter());
        }
        
        if (shape) {
            shape->setRotation(angleX, angleY, angleZ);
            shape->setScale(scale);
            shape->setNonUniformScale(nonUniformScale.x, nonUniformScale.y, nonUniformScale.z);
            shape->useUniformScaling(useUniformScaling);
            shapeManager.addShape(shape);
        }
    }
}

