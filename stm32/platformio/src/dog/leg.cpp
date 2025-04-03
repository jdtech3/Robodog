#include "dog/leg.hpp"

glm::vec3 ik_BR(const glm::vec3& r){
    float L1 = LEG_L;
    float L2 = LEG_L;
    float L1_2 = L1*L1;
    float L2_2 = L2*L2;
    float rlen = glm::length(r);
    float rlen2 = rlen*rlen;
    float a1 = glm::atan(r.x, -r.z);
    float a2 = glm::acos(glm::clamp( (L1_2 + rlen2 - L2_2)/(2*L1*rlen)  , -1.f, 1.f ));
    float a3 = glm::acos(glm::clamp( (L1_2 + L2_2 - rlen2)/(2*L1*L2)    , -1.f, 1.f ));
    float a4 = glm::acos(glm::clamp( r.y/rlen                           , -1.f, 1.f ));
    // return glm::vec3(
    //     glm::clamp(a1       , glm::radians(-45.f), glm::radians(45.f)),
    //     glm::clamp(a2+a4    , glm::radians(90.f-45.f), glm::radians(90.f+45.f)),
    //     glm::clamp(a3       , glm::radians(90.f-45.f), glm::radians(90.f+45.f))
    // );
    return glm::vec3(
        a1,
        a2+a4,
        a3
    );
}

glm::vec3 ik_FR(const glm::vec3& r){
    return ik_BR(glm::vec3(r.x, -r.y, r.z));
}

glm::vec3 ik_FL(const glm::vec3& r){
    return ik_BR(glm::vec3(-r.x, -r.y, r.z));
}

glm::vec3 ik_BL(const glm::vec3& r){
    return ik_BR(glm::vec3(-r.x, r.y, r.z));
}
