#include "ShapeManager.h"
#include "Light.h"
#include <algorithm>  // For std::find

// shapeCounter was never initialized: every shape id was seeded from whatever
// happened to be on the stack, so ids came out as large negative numbers. It
// went unnoticed because ids only need to be unique, not meaningful - but they
// are written to scene files and shown in the properties panel.
ShapeManager::ShapeManager() : selectedShape(nullptr), shapeCounter(0) {}

ShapeManager::~ShapeManager() {
    // Clean up all dynamically allocated shapes
    for (Shape* shape : shapes) {
        if (shape) {  // Ensure it's not already deleted
            delete shape;
        }
    }
    shapes.clear();  // Ensure the vector is empty after deletion
 
}

void ShapeManager::addShape(Shape* shape) {
    shapes.push_back(shape);
}

void ShapeManager::deleteShape(Shape* shape) {
    auto it = std::find(shapes.begin(), shapes.end(), shape);
    if (it != shapes.end()) {
        delete *it;              // Free the memory
        shapes.erase(it);        // Remove shape from the list
        if (selectedShape == shape) {
            selectedShape = nullptr;  // Deselect the shape if it was selected
        }
    }
}

std::vector<Shape*>& ShapeManager::getShapes() {
    return shapes;
}

void ShapeManager::setSelectedShape(Shape* shape) {
    selectedShape = shape;
}

void ShapeManager::setSelectedShapeById(int id) {
    for (Shape* shape : shapes) {
        if (shape->getId() == id) {
            selectedShape = shape;
            break;
        }
    }
}

void ShapeManager::setSelectedShapeByLastAdded() {
    selectedShape = shapes.back();
}

Shape* ShapeManager::getSelectedShape() const {
    return selectedShape;
}

Shape* ShapeManager::getShapeById(int id) const {
    for (size_t i = 0; i < shapes.size(); ++i) {
        if (shapes[i] && shapes[i]->getId() == id) return shapes[i];
    }
    return 0;
}

Shape* ShapeManager::getShapeAt(size_t index) const {
    return (index < shapes.size()) ? shapes[index] : 0;
}

int ShapeManager::getShapeCounter() {
	return shapeCounter;
}

void ShapeManager::setShapeCounter(int counter) {
	shapeCounter = counter;
}

int ShapeManager::incrementShapeCounter() {
	return ++shapeCounter;
}
	
void ShapeManager::resetAllShapes() {
    for (Shape* shape : shapes) {  // Assuming shapes is a std::vector<Shape*>
        if (shape) {
            shape->resetToDefault();
        }
    }
}

std::vector<ParticleSystem*> ShapeManager::getParticleSystems() {
    std::vector<ParticleSystem*> particleSystems;
    for (Shape* shape : shapes) {
        if (auto* particleSystem = dynamic_cast<ParticleSystem*>(shape)) {
            particleSystems.push_back(particleSystem);
        }
    }
    return particleSystems;
}

std::vector<Light*> ShapeManager::getLights() {
    std::vector<Light*> lights;
    for (Shape* shape : shapes) {
        if (auto* light = dynamic_cast<Light*>(shape)) {
            lights.push_back(light);
        }
    }
    return lights;
}
