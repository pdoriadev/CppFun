#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

Camera::Camera()
    : position(0.0f),
      target(0.0f, 0.0f, 0.0f),
      up(0.0f, 1.0f, 0.0f),
      theta(glm::pi<float>() / 2.0f),
      phi(0.0f),
      radius(5.0f),
      fovDegrees(45.0f),
      nearPlane(0.1f),
      farPlane(100.0f),
      hasOverride(false),
      viewOverride(1.0f) {
    updatePosition();
}

void Camera::updatePosition() {
    position.x = target.x + radius * std::cos(phi) * std::cos(theta);
    position.y = target.y + radius * std::sin(phi);
    position.z = target.z + radius * std::cos(phi) * std::sin(theta);
}

// orbit(), pan() and zoom() are NOT here -- they live in CameraControl.cpp,
// which is the swappable unit for the `camera` topic (A3). Everything in this
// file is given: the state those three functions manipulate, and the maths that
// turns it into a view. Splitting them out is the same surgery ShapeNormals.cpp
// needed for `geometry`. See CameraControl.cpp for why updatePosition() stayed
// behind.

void Camera::setTarget(const glm::vec3& t) {
    target = t;
    updatePosition();
}

void Camera::setRadius(float r) {
    radius = glm::max(r, 0.5f);
    updatePosition();
}

glm::mat4 Camera::getViewMatrix() const {
    if (hasOverride) return viewOverride;
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fovDegrees), aspectRatio, nearPlane, farPlane);
}

Ray Camera::generateRay(float pixelX, float pixelY, int imageWidth, int imageHeight) const {
    const float w = static_cast<float>(imageWidth);
    const float h = static_cast<float>(imageHeight);
    const float aspect = (h > 0.0f) ? (w / h) : 1.0f;

    // Half-height of the image plane at unit distance.
    const float scale = std::tan(glm::radians(fovDegrees) * 0.5f);

    // Pixel coords -> NDC in [-1, 1]. Y is flipped because pixel row 0 is the
    // top of the image while +Y points up in world space.
    const float ndcX = (2.0f * pixelX / w - 1.0f) * aspect * scale;
    const float ndcY = (1.0f - 2.0f * pixelY / h) * scale;

    // Same basis lookAt uses, so traced rays agree with the rasterized view.
    glm::vec3 forward = glm::normalize(target - position);
    glm::vec3 right   = glm::normalize(glm::cross(forward, up));
    glm::vec3 trueUp  = glm::cross(right, forward);

    glm::vec3 dir = glm::normalize(forward + right * ndcX + trueUp * ndcY);
    return Ray(position, dir);
}

void Camera::setView(float thetaRadians, float phiRadians, float distance,
                     const glm::vec3& targetPoint) {
    theta  = thetaRadians;
    phi    = glm::clamp(phiRadians,
                        -glm::half_pi<float>() + 0.001f,
                         glm::half_pi<float>() - 0.001f);
    radius = glm::max(distance, 0.5f);
    target = targetPoint;
    updatePosition();
}
