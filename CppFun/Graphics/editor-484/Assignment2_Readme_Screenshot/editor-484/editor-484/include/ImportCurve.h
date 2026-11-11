#ifndef IMPORTCURVE_H
#define IMPORTCURVE_H

#include "Curve.h"
#include "Surface.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <iostream>

class ImportCurve : public Shape {
public:
    const char* serialType() const override { return "ImportCurve"; }
    ImportCurve(float x, float y, float z, float scale, int colorIndex, int id);
    ~ImportCurve();

    // Add a new curve to this shape
    void addCurve(const Curve& curve);

    // Add a new surface to this shape
    void addSurface(const Surface& surface);

    // Set curves
    void setCurves(const std::vector<Curve>& curves);
	
    // Get all curves
    const std::vector<Curve>& getCurves() const;

    // Toggle control points visibility
    bool isControlPointsVisible() const;
    void setControlPointsVisible(bool visible);

    // Toggle curve visibility
    int getCurveVisibilityMode() const;
    void setCurveVisibilityMode(int mode);

    // Toggle surface visibility
    int getSurfaceVisibilityMode() const;
    void setSurfaceVisibilityMode(int mode);

    void printControlPointsAndCurvePoints() const;

    
    void setupCurveBuffer();
    void setupSurfaceBuffer();

    // Draw the curve (override draw from Shape)
    void draw(GLuint shaderProgram) override;

private:
    std::vector<Curve> curves;  // Stores multiple curves, each with control points, steps, and a name
    std::vector<Surface> surfaces;  // Store surfaces
	
    // Toggles for display options
    bool showControlPoints;
    int curveVisibilityMode;  // 0 = off, 1 = on, 2 = on with normals/tangents/binormals
    int surfaceVisibilityMode;  // 0 = off, 1 = wireframe, 2 = shaded

    GLuint controlPointsVAO, controlPointsVBO;
    GLuint curveVAO, curveVBO;
    GLuint vectorVAO, vectorVBO;    
    // No EBO: the surface buffer holds expanded triangle corners so the
    // texture seam repair survives (see setupSurfaceBuffer).
    GLuint surfaceVAO, surfaceVBO;
    GLsizei surfaceCornerCount;
    GLuint wireframeVAO, wireframeVBO;    
    GLuint normalVAO, normalVBO;    

};

#endif // IMPORTCURVE_H
