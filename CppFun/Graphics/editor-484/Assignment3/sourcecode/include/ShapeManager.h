#ifndef SHAPEMANAGER_H
#define SHAPEMANAGER_H

#include <vector>
#include "Shape.h"
#include "ParticleSystem.h"

class Light;

class ShapeManager {
public:
    // Constructor
    ShapeManager();

    // Destructor (will clean up dynamically allocated shapes)
    ~ShapeManager();

    // Adds a new shape to the list
    void addShape(Shape* shape);

    // Deletes a specific shape from the list
    void deleteShape(Shape* shape);

    // Returns a reference to the list of shapes
    std::vector<Shape*>& getShapes();

    // Selects a shape (by pointer, ID, or last selected)
    void setSelectedShape(Shape* shape);
    void setSelectedShapeById(int id);
	void setSelectedShapeByLastAdded();

    // Returns the currently selected shape
    Shape* getSelectedShape() const;

    // Returns the shape whose getId() matches, or null if there is none.
    //
    // This genuinely searches by id. It used to be `return shapes[id];` -- a
    // POSITIONAL lookup wearing an id-shaped name. That was survivable only
    // because both callers happened to pass a loop counter; the first caller
    // to pass a real id indexed past the end of the vector, because ids come
    // from incrementShapeCounter(), start at 1, and keep climbing as shapes
    // are added and deleted while the vector does not.
    //
    // Deleted shapes are the reason a linear search is the honest answer here:
    // ids are stable across a deletion, positions are not.
	Shape* getShapeById(int id) const;

    // Returns the shape at a position in the list, or null if out of range.
    // This is what the shape-selector panel wants.
    Shape* getShapeAt(size_t index) const;

	// Add, Get and Set shapecounter
	int getShapeCounter();
	void setShapeCounter(int counter);
	int incrementShapeCounter();
	
	// Get all particle systems in ShapeManager
	std::vector<ParticleSystem*> getParticleSystems();

	// All Light objects in the scene, in insertion order.
	std::vector<Light*> getLights();

	//Reset all shapes to default
	void resetAllShapes();

private:
    std::vector<Shape*> shapes;  // List of shapes
    Shape* selectedShape;        // Currently selected shape
	int shapeCounter;  // Keeps track of unique IDs for shapes
};

#endif  // SHAPEMANAGER_H
