#ifndef CURVE_H
#define CURVE_H

#include "Shape.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <iostream>

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V), the tangent (T), the
// normal (N), and the binormal (B).  It is the responsiblility of
// functions that create these objects to fill in all the data.

struct CurvePoint
{
    glm::vec3 V; // Vertex
    glm::vec3 T; // Tangent (unit)
    glm::vec3 N; // Normal (unit)
    glm::vec3 B; // Binormal (unit)
};

// Shortcut type definition
typedef std::vector< CurvePoint > CurvePoints;

class Curve  {
public:

    Curve(const std::string& name, unsigned steps, const std::string& curveType);
    virtual ~Curve() {}

    // Set and get control points
    void setControlPoints(const std::vector<glm::vec3>& cps);
    const std::vector<glm::vec3>& getControlPoints() const;

    // Set and get curvePoints
    void setCurvePoints(const CurvePoints& points);
    const CurvePoints& getCurvePoints() const;

    // Set and get steps
    void setSteps(unsigned steps);
    unsigned getSteps() const;

    // Getter for the curve name
    const std::string& getName() const;  // <-- Add this method

    // Set and get the curve type (e.g., "bez2", "bsp2")
    void setCurveType(const std::string& type);
    const std::string& getCurveType() const;

    // Functions to evaluate and return bezier or bspline points
    static CurvePoints evalBezier(const std::vector<glm::vec3>& P, unsigned steps);
    static CurvePoints evalBspline(const std::vector<glm::vec3>& P, unsigned steps);

    // Function to evaluate circle points
    static CurvePoints evalCircle(float radius, unsigned steps);

    // Draw function for curve (if needed)
    void draw();

protected:
    std::string name;
    unsigned steps;
    std::string curveType;
    std::vector<glm::vec3> controlPoints;
    CurvePoints curvePoints;

};

#endif  // CURVE_H
