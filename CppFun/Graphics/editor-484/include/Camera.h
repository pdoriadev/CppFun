#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include "Ray.h"

// Orbit camera, extracted from Renderer so the rasterizer and the ray tracer
// can share one definition of "where the camera is". Position is derived from
// spherical coordinates (theta, phi, radius) around a target point.
class Camera {
public:
    Camera();

    // --- orbit controls (called from mouse handling) ---
    void orbit(float dTheta, float dPhi);          // right-drag
    void pan(float dxScreen, float dyScreen);      // left-drag
    void zoom(float amount);                       // scroll wheel

    // --- accessors ---
    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getTarget()   const { return target; }
    const glm::vec3& getUp()       const { return up; }

    void setTarget(const glm::vec3& t);
    void setRadius(float r);
    float getRadius() const { return radius; }

    // The orbit angles themselves. Saving only position/target/radius is not
    // enough to restore a view: position is derived from these, and nothing
    // recovers theta and phi from it, so a reloaded scene would snap back to
    // the default angle.
    float getTheta() const { return theta; }
    float getPhi()   const { return phi; }

    // Jump straight to an orientation, used by the View menu presets.
    void setView(float thetaRadians, float phiRadians, float distance,
                 const glm::vec3& targetPoint = glm::vec3(0.0f));

    float getFovDegrees() const { return fovDegrees; }
    float getNearPlane()  const { return nearPlane; }
    float getFarPlane()   const { return farPlane; }

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    // Head tracking can temporarily replace the orbit view without changing
    // the orbit state. Clearing the override restores mouse control exactly
    // where the user left it.
    void setViewOverride(const glm::mat4& view) { viewOverride = view; hasOverride = true; }
    void clearViewOverride() { hasOverride = false; }
    bool hasViewOverride() const { return hasOverride; }

    // Builds a primary ray through a point in pixel coordinates, where
    // (0,0) is the top-left corner of the image. Pass x+0.5 / y+0.5 to aim at
    // a pixel center; pass raw cursor coordinates when picking.
    //
    // The basis is derived the same way glm::lookAt does, so rays line up with
    // what the rasterizer draws.
    Ray generateRay(float pixelX, float pixelY, int imageWidth, int imageHeight) const;

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    float theta;   // yaw, radians
    float phi;     // pitch, radians
    float radius;  // distance from target

    float fovDegrees;
    float nearPlane;
    float farPlane;

    bool hasOverride;
    glm::mat4 viewOverride;

    void updatePosition();
};

#endif // CAMERA_H
