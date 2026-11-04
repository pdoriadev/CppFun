#include "HeadPose.h"

// Given. The "no tracking" pose, so nothing downstream has to handle a null.
//
// The identity head transform and zero eye offsets are not meaningful -- `valid`
// is false and every consumer is required to check it first -- but they are
// finite, which matters: a HeadPose full of NaN would sail through a careless
// check and poison a view matrix somewhere far away from here.
HeadPose noHeadPose() {
    HeadPose p;
    p.valid              = false;
    p.headToPlaySpace    = glm::mat4(1.0f);
    p.eyeOffsetLocal[0]  = glm::vec3(-0.032f, 0.0f, 0.0f);
    p.eyeOffsetLocal[1]  = glm::vec3( 0.032f, 0.0f, 0.0f);
    p.eyeProjection[0]   = glm::mat4(1.0f);
    p.eyeProjection[1]   = glm::mat4(1.0f);
    p.ipdMetres          = 0.064f;
    return p;
}
