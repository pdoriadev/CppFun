// =============================================================================
// RayTracer::shade -- the Whitted shading model
// =============================================================================
// Split out of RayTracer.cpp deliberately. That file keeps everything that is
// always given: begin/step/cancel, the scanline loop and its time budget, the
// preview texture, image saving. This file holds only shade(), which is the
// student's work: direct lighting, shadow rays, reflection, refraction and the
// recursion that ties them together.
//
// The split is what makes a per-topic solution library possible: the unit the
// linker can swap is a whole translation unit, so the swappable code has to
// live in a file of its own. See SOLUTION-LIBRARIES.md and ASSIGNMENTS.md.
//
// NOTE FOR THE INSTRUCTOR: this is the largest spoiler risk in the course --
// 15% of the grade with a complete reference implementation in the repo. Ray
// triangle intersection and BVH traversal are still in BVH.cpp and are NOT
// covered by this topic; if they are meant to be part of the assignment they
// need a topic of their own. See ASSIGNMENTS.md, A6.

#include "RayTracer.h"
#include "Lighting.h"
#include "TextureCache.h"

#include "Shape.h"
#include "SceneLight.h"

#include <cmath>
#include <algorithm>

namespace {

// Schlick's approximation of the Fresnel reflectance.
float schlick(float cosTheta, float n1, float n2) {
    float r0 = (n1 - n2) / (n1 + n2);
    r0 = r0 * r0;
    float m = 1.0f - cosTheta;
    return r0 + (1.0f - r0) * m * m * m * m * m;
}

} // namespace

glm::vec3 RayTracer::shade(const Hit& hit, const Ray& ray, int depth) const {
    // TODO(raytracer): the Whitted shading model: direct light, shadows, reflection, refraction
    // Return the colour seen along `ray` where it met the surface `hit`.
    // Build it in this order; each step is a checkpoint that renders.
    //
    // 1. DIRECT LIGHT. For every light in the scene, work out its geometry
    //    and add its Phong contribution. lightGeometry() and
    //    phongContribution() are YOUR functions from the lighting topic --
    //    this is where they get used in anger. Getting only this far gives
    //    a lit, shadowless, matte image, which is a correct ray CASTER.
    //
    // 2. SHADOWS. One shadow ray per light, towards it, out to the light's
    //    distance. Pass the result in as phongContribution's `reach`.
    //    Use transmittance() rather than occluded() so glass casts a soft
    //    shadow instead of a brick's.
    //
    // 3. REFLECTION AND REFRACTION. Recurse, then blend with the Fresnel
    //    term (schlick() above is given). Reflected colour must be BLENDED
    //    with the surface by its reflectivity, not substituted for it: a
    //    30%-reflective object is not a mirror.
    //
    // STEPS 2 AND 3 ARE Mode_RayTrace ONLY. A ray caster shoots primary
    // rays and nothing else -- no shadow rays either -- so its output must
    // not change when the shadow/reflection/refraction toggles move. The
    // grader checks exactly that, and separately that the two modes do not
    // produce the same image. Each of the three also has its own settings
    // flag to respect on top of the mode.
    //
    // TEST `depth` BEFORE casting the first secondary ray, not after. The
    // budget is meant to be spendable down to zero, and the grader checks
    // that maxDepth = 0 renders identically to reflections turned off.
    //
    // OFFSET EVERY SECONDARY RAY ALONG hit.geoNormal, THE GEOMETRIC NORMAL,
    // oriented onto the viewer's side -- not along hit.normal. The
    // interpolated shading normal is the obvious choice and is subtly
    // wrong: near a silhouette it can lie almost in the triangle's plane,
    // so the nudge slides along the face instead of clearing it, and the
    // ray re-hits the surface it just left. That is what surface acne is.
    (void)hit; (void)ray; (void)depth;
    return glm::vec3(0.0f);
}
