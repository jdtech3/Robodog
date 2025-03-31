#ifndef QUAT_HERLPER__H_
#define QUAT_HERLPER__H_

#include <cmath>
#include <helper_3dmath.h>

Quaternion operator*(float f, Quaternion q);
Quaternion log(Quaternion q);
Quaternion exp(Quaternion q);
Quaternion pow(const Quaternion& q, float n);

#endif