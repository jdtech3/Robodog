#include "quat_helper.hpp"

Quaternion operator*(float f, Quaternion q){
    q.w *= f;
    q.x *= f;
    q.y *= f;
    q.z *= f;
    return q;
}

Quaternion log(Quaternion q){
    float magq = q.getMagnitude();
    float magv = std::sqrt(q.x*q.x + q.y*q.y + q.z*q.z);
    float m = std::acos(q.w/magq)/magv;
    q.w = std::log(magq);
    q.x *= m;
    q.y *= m;
    q.z *= m;
    return q;
}

Quaternion exp(Quaternion q){
    float magv = std::sqrt(q.x*q.x + q.y*q.y + q.z*q.z);
    float n = std::exp(q.w);
    float m = n*std::sin(magv)/magv;
    q.w = n*std::cos(magv);
    q.x *= m;
    q.y *= m;
    q.z *= m;
    return q;
}

Quaternion pow(const Quaternion& q, float n){
    return exp(n*log(q));
}