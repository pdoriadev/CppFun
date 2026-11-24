#include "Surface.h"

namespace {
// Check if the profile curve is flat on the xy-plane
bool checkFlat(const Curve &profile) {
    for (const auto& p : profile.getControlPoints()) {
        if (p.z != 0.0f) return false;
    }
    return true;
}
}

Surface Surface::makeSurfRev(const Curve& profile, unsigned steps) {
    // NOT PART OF THIS COURSE -- this is curves material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // For each of `steps` rotations about Y, transform every profile point
    // and its normal, then stitch consecutive rings into triangles. Normals
    // transform by the inverse transpose, not the matrix itself.
    (void)profile; (void)steps;
    return Surface();
}


Surface Surface::makeGenCyl(const Curve& profile, const Curve& sweep) {
    // NOT PART OF THIS COURSE -- this is curves material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // At each point of the sweep curve, place the profile in the local
    // (N, B, T) frame and transform it into world space. This is why
    // evalBezier must produce a non-flipping frame: a twist here is a twist
    // in the surface.
    (void)profile; (void)sweep;
    return Surface();
}


