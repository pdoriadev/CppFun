// =============================================================================
// Skeletal animation -- joint transforms and skin deformation
// =============================================================================
// Split out of SkeletalModel.cpp and ImportCharacter.cpp deliberately. Those
// files keep everything that is always given: joint storage and hierarchy,
// setJointTransform, the MatrixStack, the .skel/.attach loading, and roughly
// 300 lines of VAO setup and mesh/joint/bone drawing. This file holds the
// three functions that are the student's work.
//
// The split is what makes a per-topic solution library possible: the unit the
// linker can swap is a whole translation unit, so the swappable code has to
// live in a file of its own. See SOLUTION-LIBRARIES.md and ASSIGNMENTS.md.
//
// The three functions are one pipeline, and they have to be understood in
// order:
//
//   computeBindWorldToJointTransforms   runs ONCE. For each joint, the inverse
//                                       of its world transform in the bind
//                                       pose -- "where was this joint when the
//                                       mesh was authored?"
//   updateCurrentJointToWorldTransforms runs whenever a joint angle changes.
//                                       For each joint, its world transform in
//                                       the CURRENT pose.
//   updateMeshVertices                  runs after the above. For each vertex,
//                                       the weighted sum over joints of
//                                       current * bindInverse * v.
//
// Getting the two matrices the wrong way round is the classic failure and it
// looks spectacular: the mesh explodes outward instead of deforming.

#include "SkeletalModel.h"
#include "ImportCharacter.h"
#include "Joint.h"
#include "MatrixStack.h"

#include <glm/gtc/matrix_transform.hpp>


void bindWorldToJointTransformRecursive(Joint* joint, MatrixStack& myStack) {
    // NOT PART OF THIS COURSE -- this is skinning material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // Push this joint's transform onto the stack, store the inverse of the
    // accumulated matrix on the joint, recurse into the children, pop.
    (void)joint; (void)myStack;
}

void SkeletalModel::computeBindWorldToJointTransforms() {
    // NOT PART OF THIS COURSE -- this is skinning material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // Walk the joint hierarchy with the MatrixStack, accumulating each
    // joint's transform onto its parent's. Store the inverse of the
    // accumulated matrix. Runs once -- there is only one bind pose.
}

void currentJointToWorldTransformsRecursive(Joint* joint, MatrixStack& myStack) {
    // NOT PART OF THIS COURSE -- this is skinning material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // The same traversal as the bind pass, storing the accumulated matrix
    // rather than its inverse.
    (void)joint; (void)myStack;
}

void SkeletalModel::updateCurrentJointToWorldTransforms() {
    // NOT PART OF THIS COURSE -- this is skinning material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // The same traversal, but store the accumulated matrix itself rather
    // than its inverse. Runs every time a joint angle changes.
}

void ImportCharacter::updateMeshVertices() {
    // NOT PART OF THIS COURSE -- this is skinning material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // For each bind vertex, sum over the joints it is attached to:
    //     weight * current * bindInverse * v
    // then write the result into `vertices` and call calculateNormals().
    // Attachments skip the root, so attachment a refers to joints[a + 1].
    // Getting current and bindInverse the wrong way round makes the mesh
    // explode outward rather than deform.
    std::vector<glm::vec3> newVertices(bindVertices.size());
    vertices = newVertices;
    calculateNormals();

    // Given, and deliberately OUTSIDE the marker: re-uploading the deformed
    // mesh is plumbing, not the assignment. Left inside, a student with
    // correct skinning maths would see nothing change on screen and have no
    // way to tell why.
    setupMeshBuffer();
    setupJointBuffer();
    setupBoneBuffer();
}
