#include "ImportCurve.h"
#include "TexCoords.h"

ImportCurve::ImportCurve(float x, float y, float z, float scale, int colorIndex, int id)
    : Shape(x, y, z, scale, colorIndex, id), showControlPoints(true), curveVisibilityMode(1), surfaceVisibilityMode(2), 
       controlPointsVAO(0), controlPointsVBO(0), curveVAO(0), curveVBO(0), vectorVAO(0), vectorVBO(0), surfaceVAO(0), surfaceVBO(0), surfaceCornerCount(0), wireframeVAO(0), wireframeVBO(0), normalVAO(0), normalVBO(0) {
}

ImportCurve::~ImportCurve() {

    // Clear existing data
    glDeleteVertexArrays(1, &controlPointsVAO);
    glDeleteBuffers(1, &controlPointsVBO);

    glDeleteVertexArrays(1, &curveVAO);
    glDeleteBuffers(1, &curveVBO);

    glDeleteVertexArrays(1, &vectorVAO);
    glDeleteBuffers(1, &vectorVBO);

    glDeleteVertexArrays(1, &surfaceVAO);
    glDeleteBuffers(1, &surfaceVBO);
    
    glDeleteVertexArrays(1, &wireframeVAO);
    glDeleteBuffers(1, &wireframeVBO);

    glDeleteVertexArrays(1, &normalVAO);
    glDeleteBuffers(1, &normalVBO);

}

void ImportCurve::printControlPointsAndCurvePoints() const {
    std::cout << "Control Points and Curve Points:" << std::endl;
    std::cout << "Curve Number: " << curves.size() << std::endl;    
    for (const auto& curve : curves) {
        std::cout << "Control Point Number: " << curve.getControlPoints().size() << std::endl;    
        std::cout << "Curve: " << std::endl;
        for (const auto& point : curve.getControlPoints()) {
            std::cout << "Control Point - X: " << point.x << " Y: " << point.y << " Z: " << point.z << std::endl;
        }
        for (const auto& point : curve.getCurvePoints()) {
            std::cout << "Curve Point - X: " << point.V.x << " Y: " << point.V.y << " Z: " << point.V.z << std::endl;
        }
    }
}

// Add a new curve to the list
void ImportCurve::addCurve(const Curve& curve) {
    curves.push_back(curve);  
}

// Add a new surface to the list
void ImportCurve::addSurface(const Surface& surface) {
    surfaces.push_back(surface);

    // Mirror the surface into Shape's own vertex/normal/face arrays.
    //
    // Those arrays are what everything OUTSIDE the rasterizer reads: the ray
    // tracer, the picker, and the bounding-box code all go through
    // getVertices()/getFaces() rather than touching a VAO. ImportCurve used to
    // keep its geometry solely in `surfaces` and hand it straight to GL, so a
    // swept surface drew in the viewport and then vanished from a trace --
    // RayScene::build saw an empty vertex list and skipped the shape entirely.
    //
    // Appending rather than assigning, with `base` offsetting the indices, is
    // what lets one ImportCurve hold several surfaces: each surface's face
    // indices are local to its own VV, so they have to be rebased as they are
    // concatenated. This mirrors the offset walk setupSurfaceBuffer() already
    // does for the wireframe.
    //
    // Normals stay parallel to vertices, which is the condition RayScene checks
    // before trusting them for smooth shading -- a swept surface has genuine
    // per-vertex normals from the sweep, and they are worth keeping.
    const size_t base = vertices.size();

    vertices.insert(vertices.end(), surface.VV.begin(), surface.VV.end());

    // Guard the sizes rather than assuming: a malformed surface with fewer
    // normals than vertices would otherwise break the parallel-array invariant
    // for every surface added after it, not just its own.
    if (surface.VN.size() == surface.VV.size()) {
        normals.insert(normals.end(), surface.VN.begin(), surface.VN.end());
    } else {
        normals.resize(vertices.size(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    faces.reserve(faces.size() + surface.VF.size());
    for (size_t i = 0; i < surface.VF.size(); ++i) {
        const Tup3u& f = surface.VF[i];

        // Skip a face that points outside its own surface instead of pushing an
        // out-of-range index into the shared array, where it would abort in the
        // tracer far from the cause.
        if (f.v[0] >= surface.VV.size() ||
            f.v[1] >= surface.VV.size() ||
            f.v[2] >= surface.VV.size()) {
            continue;
        }

        std::vector<int> face;
        face.reserve(3);
        face.push_back(static_cast<int>(base + f.v[0]));
        face.push_back(static_cast<int>(base + f.v[1]));
        face.push_back(static_cast<int>(base + f.v[2]));
        faces.push_back(face);
    }

    // Cylindrical UVs about Y, one per triangle corner, seam repaired.
    //
    // Recomputed over the whole shape rather than appended per surface: the
    // projection normalises v by the mesh's Y bounds, and a second surface can
    // extend those bounds, which would leave the first surface's UVs keyed to a
    // range that no longer exists.
    //
    // A surface of revolution IS a cylinder in the only sense that matters here,
    // so this is its natural parameterisation rather than a fallback. A swept
    // generalized cylinder that doubles back on itself will show stretching --
    // an honest limitation of projecting rather than using the sweep's own
    // parameters, which are not recoverable from the finished triangle list.
    //
    // Note where this lives: ImportCurve.cpp is given, so a 484 student gets
    // textured curve surfaces even though makeSurfRev and makeGenCyl are 566's
    // assignment and are stubbed out in their tree.
    texCoords = TexCoords::cylindricalPerCorner(vertices, faces);
}

// Get all the curves in this shape
const std::vector<Curve>& ImportCurve::getCurves() const {
    return curves;
}

void ImportCurve::setCurves(const std::vector<Curve>& curves) {
    this->curves = curves;
}

bool ImportCurve::isControlPointsVisible() const {
    return showControlPoints;
}

void ImportCurve::setControlPointsVisible(bool visible) {
    showControlPoints = visible;
}

int ImportCurve::getCurveVisibilityMode() const {
    return curveVisibilityMode;
}

void ImportCurve::setCurveVisibilityMode(int mode) {
    curveVisibilityMode = mode;
}

int ImportCurve::getSurfaceVisibilityMode() const {
    return surfaceVisibilityMode;
}

void ImportCurve::setSurfaceVisibilityMode(int mode) {
    surfaceVisibilityMode = mode;
}

void ImportCurve::setupCurveBuffer() {

    // Clear existing data
    glDeleteVertexArrays(1, &controlPointsVAO);
    glDeleteBuffers(1, &controlPointsVBO);

    glDeleteVertexArrays(1, &curveVAO);
    glDeleteBuffers(1, &curveVBO);

    glDeleteVertexArrays(1, &vectorVAO);
    glDeleteBuffers(1, &vectorVBO);


    // Collect vertices for control points, control lines, and curve lines
    std::vector<float> controlVertices;
    std::vector<float> curveVertices;
    std::vector<float> vectorVertices;

    for (const auto& curve : curves) {
  
        // Control Points and Control Lines
        for (const auto& point : curve.getControlPoints()) {
            controlVertices.insert(controlVertices.end(), {point.x, point.y, point.z, 1.0f, 1.0f, 0.0f});
        }

        // Curve Lines
        for (const auto& point : curve.getCurvePoints()) {
            curveVertices.insert(curveVertices.end(), {point.V.x, point.V.y, point.V.z, 1.0f, 1.0f, 1.0f});
        }
        
        // Tangent, Normal, and Binormal Lines
        for (const auto& point : curve.getCurvePoints()) {
            // Scale for visibility
            float scale = 0.2f;
            
            // Tangent (Blue)
            glm::vec3 tangentEnd = point.V + point.T * scale;
            vectorVertices.insert(vectorVertices.end(), {
                // Start Point
                point.V.x, point.V.y, point.V.z,   0.0f, 0.0f, 1.0f, // Blue
                // End Point
                tangentEnd.x, tangentEnd.y, tangentEnd.z, 0.0f, 0.0f, 1.0f // Blue
            });

            // Normal (Red)
            glm::vec3 normalEnd = point.V + point.N * scale;
            vectorVertices.insert(vectorVertices.end(), {
                point.V.x, point.V.y, point.V.z,   1.0f, 0.0f, 0.0f, // Red
                normalEnd.x, normalEnd.y, normalEnd.z, 1.0f, 0.0f, 0.0f // Red
            });

            // Binormal (Green)
            glm::vec3 binormalEnd = point.V + point.B * scale;
            vectorVertices.insert(vectorVertices.end(), {
                point.V.x, point.V.y, point.V.z,   0.0f, 1.0f, 0.0f, // Green
                binormalEnd.x, binormalEnd.y, binormalEnd.z, 0.0f, 1.0f, 0.0f // Green
            });
            
        }
       
    }

    // Setup VAO and VBO for Control Points and Control Lines
    glGenVertexArrays(1, &controlPointsVAO);
    glGenBuffers(1, &controlPointsVBO);

    glBindVertexArray(controlPointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, controlPointsVBO);
    glBufferData(GL_ARRAY_BUFFER, controlVertices.size() * sizeof(float), controlVertices.data(), GL_STATIC_DRAW);

    // Positions -> location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Colors -> location 2
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Setup VAO and VBO for Curve Lines
    glGenVertexArrays(1, &curveVAO);
    glGenBuffers(1, &curveVBO);

    glBindVertexArray(curveVAO);
    glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
    glBufferData(GL_ARRAY_BUFFER, curveVertices.size() * sizeof(float), curveVertices.data(), GL_STATIC_DRAW);

    // Positions -> location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Colors -> location 2
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    
    // Setup VAO and VBO for all Vectors (Tangents, Normals, Binormals)
    glGenVertexArrays(1, &vectorVAO);
    glGenBuffers(1, &vectorVBO);

    glBindVertexArray(vectorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vectorVBO);
    glBufferData(GL_ARRAY_BUFFER, vectorVertices.size() * sizeof(float), vectorVertices.data(), GL_STATIC_DRAW);

    // Positions -> location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Colors -> location 2
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);  
    
}


void ImportCurve::setupSurfaceBuffer() {

    // Clear existing data
    if (surfaceVAO) glDeleteVertexArrays(1, &surfaceVAO);
    if (surfaceVBO) glDeleteBuffers(1, &surfaceVBO);
    if (wireframeVAO) glDeleteVertexArrays(1, &wireframeVAO);
    if (wireframeVBO) glDeleteBuffers(1, &wireframeVBO);
    if (normalVAO) glDeleteVertexArrays(1, &normalVAO);
    if (normalVBO) glDeleteBuffers(1, &normalVBO);

    // Collect vertices, normals, colors and UVs.
    //
    // Emitted PER TRIANGLE CORNER rather than indexed, which is a deliberate
    // change from the EBO this used to draw with. Indexed drawing indexes every
    // attribute together, so a shared vertex must have one UV -- and the wrap
    // seam needs the opposite: a corner on the seam wants u just past 1.0 in one
    // triangle and just above 0.0 in its neighbour. Expanding here is what lets
    // TexCoords::cylindricalPerCorner's seam repair survive to the GPU, and it
    // costs one extra copy of a mesh that is rebuilt only when the curve changes.
    //
    // It also fixes a latent bug: draw() bound this VAO once per surface and
    // drew each surface's index count from offset 0, so a shape with two
    // surfaces drew the first one twice.
    std::vector<float> surfaceVertices;
    std::vector<float> wireframeVertices;
    std::vector<float> normalLines;

    // texCoords is per corner over the shape's whole face list (see
    // addSurface), so it is walked with one running counter across surfaces
    // rather than restarted for each.
    size_t corner = 0;

    for (const auto& surface : surfaces) {
        for (const auto& face : surface.VF) {
            for (int i = 0; i < 3; i++) {
                if (face.v[i] >= surface.VV.size()) { ++corner; continue; }

                const glm::vec3 position = surface.VV[face.v[i]];
                const glm::vec3 normal =
                    (face.v[i] < surface.VN.size()) ? surface.VN[face.v[i]]
                                                    : glm::vec3(0.0f, 1.0f, 0.0f);
                const glm::vec2 uv = (corner < texCoords.size()) ? texCoords[corner]
                                                                 : glm::vec2(0.0f);
                ++corner;

                surfaceVertices.insert(surfaceVertices.end(), {
                    position.x, position.y, position.z,
                    normal.x, normal.y, normal.z,
                    0.27f, 0.51f, 0.71f,
                    uv.x, uv.y
                });

                wireframeVertices.insert(wireframeVertices.end(), {position.x, position.y, position.z});
                wireframeVertices.insert(wireframeVertices.end(), { 0.27f, 0.51f, 0.71f});  // Light Blue Wireframe
            }
        }
        
        // Normal Lines
        for (size_t i = 0; i < surface.VV.size(); i++) {
            glm::vec3 start = surface.VV[i];
            glm::vec3 end = start + surface.VN[i] * 0.1f; // Scale normal length for visibility

            normalLines.insert(normalLines.end(), {start.x, start.y, start.z, 0.0f, 0.50f, 0.50f});
            normalLines.insert(normalLines.end(), {end.x, end.y, end.z, 0.0f, 0.50f, 0.50f});
        }        
    }

    // Setup VAO and VBO for Surface. Stride 11: position, normal, colour, UV.
    // Every other shape keeps UVs in a second VBO on attribute 3 because their
    // interleaved buffers were already written; this one is being rewritten
    // here, so interleaving costs nothing and keeps the corner expansion in one
    // place.
    surfaceCornerCount = static_cast<GLsizei>(surfaceVertices.size() / 11);

    glGenVertexArrays(1, &surfaceVAO);
    glGenBuffers(1, &surfaceVBO);

    glBindVertexArray(surfaceVAO);

    glBindBuffer(GL_ARRAY_BUFFER, surfaceVBO);
    glBufferData(GL_ARRAY_BUFFER, surfaceVertices.size() * sizeof(float), surfaceVertices.data(), GL_STATIC_DRAW);

    // Positions -> location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normals -> location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Colors -> location 2
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Texture coordinates -> location 3
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    
    // Setup VAO/VBO for Wireframe
    glGenVertexArrays(1, &wireframeVAO);
    glGenBuffers(1, &wireframeVBO);

    glBindVertexArray(wireframeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, wireframeVBO);
    glBufferData(GL_ARRAY_BUFFER, wireframeVertices.size() * sizeof(float), wireframeVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Setup VAO/VBO for Normals
    glGenVertexArrays(1, &normalVAO);
    glGenBuffers(1, &normalVBO);

    glBindVertexArray(normalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normalLines.size() * sizeof(float), normalLines.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);    

}



void ImportCurve::draw(GLuint shaderProgram) {

    // Counter for space needed for vertices
    int totalVertices = 0;

    // Use the shader program
    glUseProgram(shaderProgram);

    // Disable lighting for the curves and control points
    GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 0); 
    }

    // Apply transformations and pass to the shader
    applyTransform(shaderProgram);

    // Draw Control Points and Lines Connecting Them
    if (showControlPoints) {
        glPointSize(5.0f);
        glBindVertexArray(controlPointsVAO);

        int offset = 0;
        for (const auto& curve : curves) {
            int numPoints = static_cast<GLsizei>(curve.getControlPoints().size());

            // Draw Control Points as GL_POINTS (Yellow)
            glUniform3f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 1.0f, 0.0f); // Yellow
            glDrawArrays(GL_POINTS, offset, numPoints);

            // Draw Lines Connecting Control Points for This Curve
            glUniform3f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 1.0f, 0.0f); // Yellow
            glDrawArrays(GL_LINE_STRIP, offset, numPoints);

            // Move to the next curve's control points
            offset += numPoints;
        }

        glBindVertexArray(0);
    }

    // Draw each curve separately
    if (curveVisibilityMode > 0) {
        glBindVertexArray(curveVAO);

        int offset = 0;
        for (const auto& curve : curves) {
            int numPoints = static_cast<GLsizei>(curve.getCurvePoints().size());

            // Draw each curve as a separate line strip
            glDrawArrays(GL_LINE_STRIP, offset, numPoints);

            // Move to the next curve's points
            offset += numPoints;
            
            // Calculate size for vertices
            int numVerticePoints = static_cast<int>(curve.getCurvePoints().size());
            totalVertices += numVerticePoints * 3 * 2;            
        }

        glBindVertexArray(0);
    }
    
    if (curveVisibilityMode == 2) {
        glBindVertexArray(vectorVAO);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(totalVertices));
        glBindVertexArray(0);
    }

    if (surfaceVisibilityMode == 1) {

        // Disable lighting for wireframe and normals
        GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
        if (lightingLoc != -1) {
            glUniform1i(lightingLoc, 0);
        }

        applyTransform(shaderProgram);

        // Compute total vertex counts for all surfaces
        GLsizei wireframeVertexCount = 0;
        GLsizei normalVertexCount = 0;

        for (const auto& surface : surfaces) {
            wireframeVertexCount += static_cast<GLsizei>(surface.VF.size() * 3);  // 3 vertices per face
            normalVertexCount += static_cast<GLsizei>(surface.VN.size() * 2);  // Each normal needs 2 points (start & end)
        }

        // Draw wireframe
        glLineWidth(0.1f);
        glBindVertexArray(wireframeVAO);
        glDrawArrays(GL_LINES, 0, wireframeVertexCount);
        glBindVertexArray(0);

        // Draw normals
        glLineWidth(0.1f);
        glBindVertexArray(normalVAO);
        glDrawArrays(GL_LINES, 0, normalVertexCount);
        glBindVertexArray(0);

        // Restore lighting
        if (lightingLoc != -1) {
            glUniform1i(lightingLoc, 1);
        }


    } else if (surfaceVisibilityMode == 2) {
        
        // Enable lighting for the surface
        GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
        if (lightingLoc != -1) {
            glUniform1i(lightingLoc, 1); // Enable lighting for the surface
        }

        // Apply transformations and pass to the shader
        applyTransform(shaderProgram);

        // Pass material properties
        GLint colorLoc = glGetUniformLocation(shaderProgram, "material.color");
        
        if (colorLoc != -1) {
            glUniform3fv(colorLoc, 1, (colorIndex == 31) ? customColor : colorPresets[colorIndex].color);
        }

        // Binds the albedo texture and sets the hasTexture gate. Without this
        // the surface has UVs and a texture assigned and still renders untextured
        // -- the shader never learns there is anything to sample.
        applyMaterial(shaderProgram);

        // One draw for every surface: the buffer holds all of them expanded to
        // triangle corners, back to back. (The old loop rebound the same VAO
        // per surface and drew each one's count from offset 0, which drew the
        // first surface once per surface present.)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(surfaceVAO);
        glDrawArrays(GL_TRIANGLES, 0, surfaceCornerCount);
        glBindVertexArray(0);

        // Disable lighting after drawing the surface (for axis rendering)
        if (lightingLoc != -1) {
            glUniform1i(lightingLoc, 0);
        }
    } 
}

