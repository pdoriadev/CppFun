#include "Curve.h"
// glm 1.0+ hard-errors on gtx/ headers unless experimental extensions are
// explicitly enabled. Older glm (0.9.9, what Ubuntu ships) tolerated it, so
// this only shows up on newer toolchains such as MSYS2's glm 1.0.3.
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/string_cast.hpp>

Curve::Curve(const std::string& name, unsigned steps, const std::string& curveType)
    : name(name), steps(steps), curveType(curveType) {}

// Get curve name
const std::string& Curve::getName() const {
    return name;
}

// Set control points
void Curve::setControlPoints(const std::vector<glm::vec3>& cps) {
    controlPoints = cps;
}

// Get control points
const std::vector<glm::vec3>& Curve::getControlPoints() const {
    return controlPoints;
}

// Getter for curvePoints
const CurvePoints& Curve::getCurvePoints() const {
    return curvePoints;
}

// Setter for curvePoints
void Curve::setCurvePoints(const CurvePoints& points) {
    curvePoints = points;
}


// Set steps
void Curve::setSteps(unsigned steps) {
    this->steps = steps;
}

// Get steps
unsigned Curve::getSteps() const {
    return steps;
}

// Set curve type
void Curve::setCurveType(const std::string& type) {
    curveType = type;
}

// Get curve type
const std::string& Curve::getCurveType() const {
    return curveType;
}








// Function to evaluate circle points
CurvePoints Curve::evalCircle(float radius, unsigned steps) {
    CurvePoints circlePoints;
    float angleStep = (2.0f * glm::pi<float>()) / steps;
    
    for (unsigned i = 0; i <= steps; ++i) {
        float angle = i * angleStep;
        glm::vec3 V(radius * cos(angle), radius * sin(angle), 0);
        glm::vec3 T(-sin(angle), cos(angle), 0);
        glm::vec3 N(-cos(angle), -sin(angle), 0);
        glm::vec3 B(0, 0, 1);
        circlePoints.push_back({V, T, N, B});
    }
    return circlePoints;
}
