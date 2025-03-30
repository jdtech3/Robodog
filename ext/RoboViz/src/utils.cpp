#include "utils.hpp"

float MotionVar::rotFactor = 0.f;

MotionVar::MotionVar(float val, const Bounds& pBounds):
    desired{val},
    actual{val},
    pBounds{pBounds}
{}

void MotionVar::incrementDesired(){
    desired += 1;
    if(pBounds.max != 0 && desired >= pBounds.max){
        desired -=  distanceBetweenBounds();
        actual -= distanceBetweenBounds();
    }
}

void MotionVar::decrementDesired(){
    desired -= 1;
    if(pBounds.min != 0 && desired <= pBounds.min){
        desired += distanceBetweenBounds();
        actual += distanceBetweenBounds();
    }
}

void MotionVar::update(){
    float delta = desired - actual;
    if(std::abs(delta) > CLAMP_THRESHOLD) actual += rotFactor*delta;
    else actual = desired;
}

void MotionVar::update_factor(float fps){
    rotFactor = (std::log(1.f/CLAMP_THRESHOLD)/CONVERGENCE_TIME)/fps;
}


MotionVector3D::MotionVector3D(float x, float y, float z, const MotionVar::Bounds& pBounds):
    x{x, pBounds},
    y{y, pBounds},
    z{z, pBounds}
{}

MotionVector3D::MotionVector3D(glm::vec3 v, const MotionVar::Bounds& pBounds):
    x{v.x, pBounds},
    y{v.y, pBounds},
    z{v.z, pBounds}
{}

void MotionVector3D::reset(float x, float y, float z) {
    this->x.desired = x;
    this->y.desired = y;
    this->z.desired = z;
}

void MotionVector3D::computeMotion(){
    x.update();
    y.update();
    z.update();
}

Camera::Camera(const glm::mat4& proj):
    observerPosition{ 0.f, 0.f, 5.f, {-50.f, 50.f}},
    observerForwards{ 0.f, 0.f, -1.f, {-50.f, 50.f}},
    observerUpwards{ 0.f, 1.f, 0.f, {-50.f, 50.f}},
    rotationState{ 0.f,  0.f, 0.f, {-KEYHITS_PER_ROTATION/2.f, KEYHITS_PER_ROTATION/2.f}},
    proj{proj}
{}

glm::mat4 Camera::get_vp() const{
    return proj*
    glm::lookAt(
        observerPosition.getCurrent(),
        observerPosition.getCurrent() + observerForwards.getCurrent(),
        observerUpwards.getCurrent()
    );
}