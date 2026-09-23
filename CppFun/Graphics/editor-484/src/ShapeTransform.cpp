// =============================================================================
// Shape::getModelMatrix -- the model transform
// =============================================================================
// Split out of Shape.cpp deliberately. That file keeps everything that is
// always given: materials, colour, vertex and normal accessors, texture
// coordinate upload, reset-to-default. This file holds only the composition of
// the model matrix, which is the student's work.
//
// The split is what makes a per-topic solution library possible: the unit the
// linker can swap is a whole translation unit, so the swappable code has to
// live in a file of its own. See SO#LUTION-LIBRARIES.md and ASSIGNMENTS.md.
//
// The order is T * Rz * Ry * Rx * S, read right to left: scale the object in
// its own frame, then rotate it, then move it. Any other order gives a
// different result -- scaling after a rotation shears, and translating before
// a rotation swings the object round the origin instead of spinning in place.

#include "../include/Shape.h"

//-///////////////////////////////////////////////
// Scale first. Scale local to the model.
//      Scaling after Rotation will scale along world bases instead of the model's local bases. 
//          Causes shearing.
// Rotate next. Rotate local to the model.
//      Rotating after Scaling will rotate around world bases located at world origin. 
//          Causes "orbit" rotation around world origin instead of local rotation.
// Translate last. Moves model origin to world. 
// https://www.codinglabs.net/article_world_view_projection_matrix.aspx 
// https://learnopengl.com/Getting-Started/Coordinate-Systems
glm::mat4 Shape::getModelMatrix() const {
    // Construct position matrix
    glm::mat4 m4_pos(1.0f);
    m4_pos[3] = glm::vec4(x, y, z, 1.0f);
    
    // Construct scale matrix
    glm::mat4 m4_scale = glm::mat4(1.0f) * scale;
    if (useUniformScale == false) {
        m4_scale[0] *= scaleX;
        m4_scale[1] *= scaleY;
        m4_scale[2] *= scaleZ;
        // m4_scale[0] = glm::vec4(scaleX, 0.0f, 0.0f, 0.0f);
        // m4_scale[1] = glm::vec4(0.0f, scaleY, 0.0f, 0.0f);
        // m4_scale[2] = glm::vec4(0.0f, 0.0f, scaleZ, 0.0f);
    }
    
    // Construct rotation matrices
    glm::mat4 x_rot(1.0f);
    x_rot[1] = glm::vec4(0.0f, cos(angleX), sin(angleX), 0.0f);
    x_rot[2] = glm::vec4(0.0f, -sin(angleX), cos(angleX), 0.0f);

    glm::mat4 y_rot(1.0f);
    y_rot[0] = glm::vec4(cos(angleY), 0.0f, -sin(angleY), 0.0f);
    y_rot[2] = glm::vec4(sin(angleY), 0.0f, cos(angleY), 0.0f);

    glm::mat4 z_rot(1.0f);
    z_rot[0] = glm::vec4(cos(angleZ), -sin(angleZ), 0.0f, 0.0f);
    z_rot[1] = glm::vec4(sin(angleZ), cos(angleZ), 0.0f, 0.0f);
    
    // Combine rotation matrices into one matrix
    glm::mat4 m4_rot = x_rot * y_rot * z_rot;

    // glm::mat4 m4_test = m4_pos * x_rot * y_rot * z_rot * m4_scale;

    // Compute model matrix
    return m4_pos * m4_rot * m4_scale;
    
    //-/////////////////////////////////////////////////
    // ORIGINAL COMMENTS 
    //
    // TODO(transforms): compose the model matrix as T * Rx * Ry * Rz * S
    // Return the 4x4 matrix that takes this shape from its own space into
    // the world, accounting for position, rotation about all three axes, and
    // scale.
    //
    // Build the matrices YOURSELF. glm::mat4 and glm::vec3 as types are fine,
    // and so is * to multiply them. glm::translate, glm::rotate and
    // glm::scale are not -- the point is to know what is inside them.
    //
    // Three things to get right:
    //   ORDER. Matrix multiplication does not commute, and the rightmost
    //     factor applies first. Scaling then rotating is not the same as
    //     rotating then scaling; one of them shears your shape.
    //   FRAME. A shape rotates about its OWN origin, not the world's. If
    //     your cube orbits the origin when you rotate it, your order is
    //     wrong.
    //   STORAGE. glm::mat4's constructor takes COLUMNS. The translation
    //     lives in the fourth column. Getting this backwards gives you the
    //     transpose, which looks almost right until something rotates.
    //
    // useUniformScale selects between `scale` and (scaleX, scaleY, scaleZ).
    //
    // Returning the identity leaves every shape at the origin, unrotated and
    // unscaled -- so the Insert menu still works and you can see your
    // geometry, which is the right place to start.
    // return glm::mat4(1.0f);
}

