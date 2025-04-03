#include "dog/leg.hpp"

void Leg::update_target(const glm::vec3& target_r) {
    switch (leg) {
        case BACK_LEFT: _ik_BL(target_r); break;
        case BACK_RIGHT: _ik_BR(target_r); break;
        case FRONT_LEFT: _ik_FL(target_r); break;
        case FRONT_RIGHT: _ik_FR(target_r); break;
    }
}

void Leg::step_angle() {
    servo_0->set_angle(target_joint_angle.x - JOINT_0_IK_NEUTRAL + SERVO_NEUTRAL);
    servo_1->set_angle(target_joint_angle.y - JOINT_1_IK_NEUTRAL + SERVO_NEUTRAL);
    servo_2->set_angle(target_joint_angle.z - JOINT_2_IK_NEUTRAL + SERVO_NEUTRAL);
}

void Leg::_ik_BR(glm::vec3 r){
    const float L0 = LEG_L0;
    const float L1 = LEG_L1;
    const float L2 = LEG_L2;
    const float L1_2 = L1*L1;
    const float L2_2 = L2*L2;
    const float a1 = glm::acos(L0*glm::inversesqrt(r.x*r.x + r.z*r.z)) - glm::atan(-r.z, r.x);
    r.x -= L0*glm::cos(a1);
    r.z -= L0*glm::sin(a1);
    const float rlen = glm::length(r);
    const float rlen2 = rlen*rlen;
    const float a2 = glm::acos(glm::clamp( (L1_2 + rlen2 - L2_2)/(2*L1*rlen)  , -1.f, 1.f ));
    const float a3 = glm::acos(glm::clamp( (L1_2 + L2_2 - rlen2)/(2*L1*L2)    , -1.f, 1.f ));
    const float a4 = glm::acos(glm::clamp( r.y/rlen                           , -1.f, 1.f ));

    target_joint_angle = glm::vec3(
        a1,
        a2+a4,
        a3
    );
}

void Leg::_ik_FR(glm::vec3 r){
    r.y *= -1;
    _ik_BR(r);
}

void Leg::_ik_FL(glm::vec3 r){
    r.x *= -1;
    r.y *= -1;
    _ik_BR(r);
}

void Leg::_ik_BL(glm::vec3 r){
    r.x *= -1;
    _ik_BR(r);
}
