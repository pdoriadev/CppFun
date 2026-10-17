// =============================================================================
// Curve evaluation -- Bezier and B-spline
// =============================================================================
// Split out of Curve.cpp deliberately. That file keeps everything that is
// always given: construction, the name/steps/type accessors, the control-point
// and curve-point storage, and evalCircle (a closed-form generator, not an
// algorithm to derive). This file holds the two evaluators, which are the
// student's work.
//
// The split is what makes a per-topic solution library possible: the unit the
// linker can swap is a whole translation unit, so the swappable code has to
// live in a file of its own. See SOLUTION-LIBRARIES.md and ASSIGNMENTS.md.
//
// Both functions return a CurvePoints -- a list of frames, each carrying the
// point V and an orthonormal basis (T, N, B) along the curve. The basis is
// what Surface::makeGenCyl sweeps a profile through, so a curve with a
// correct V but a garbage frame produces a correct-looking curve and a
// mangled surface. That is worth knowing before debugging assignment 3.

#include "Curve.h"

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/string_cast.hpp>

// Function to evaluate Bezier curve points
CurvePoints Curve::evalBezier(const std::vector<glm::vec3>& P, unsigned steps) {
    // NOT PART OF THIS COURSE -- this is curves material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // Control points come in groups of 3n+1. For each segment evaluate the
    // cubic at `steps` parameter values, and build (T, N, B) at each: T from
    // the derivative, then N and B by propagating the previous binormal so
    // the frame does not flip. Endpoints are interpolated; the interior
    // control points are not.
    (void)P; (void)steps;
    return CurvePoints();
}

CurvePoints Curve::evalBspline(const std::vector<glm::vec3>& P, unsigned steps) {
    // NOT PART OF THIS COURSE -- this is curves material,
    // removed so it cannot be used as an answer key.
    // The feature is inert; you are not expected to fill this in.
    // Suggested route: change basis from B-spline to Bezier and call
    // evalBezier, rather than writing a second evaluator. A B-spline does
    // NOT interpolate its control points -- it stays inside their convex
    // hull, which is the property the grader checks.
    (void)P; (void)steps;
    return CurvePoints();
}
