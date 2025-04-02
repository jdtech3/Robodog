#pragma once

#include <cmath>

constexpr long double operator"" _deg(long double deg){
    return deg * M_PI / 180;
}
