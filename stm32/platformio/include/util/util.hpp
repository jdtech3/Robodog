#pragma once

#include <cmath>
#include <type_traits>

constexpr long double operator"" _deg(long double deg){
    return deg * M_PI / 180;
}

constexpr float rad2deg(float rad) {
    return rad * 180 / M_PI;
}

// Pulled from C++20
/**
   * Linearly interpolate from @c a to @c b by fraction @c t.
   * @f[
   *    lerp(t; a, b) = t b + (1 - t) a
   * @f]
   */
template<typename _Float>
constexpr std::enable_if_t<std::is_floating_point_v<_Float>, _Float>
lerp(_Float __a, _Float __b, _Float __t) {
    if (std::isnan(__a) || std::isnan(__b) || std::isnan(__t))
	    return std::numeric_limits<_Float>::quiet_NaN();

    else if ((__a <= _Float{0} && __b >= _Float{0}) || (__a >= _Float{0} && __b <= _Float{0}))
	// ab <= 0 but product could overflow.
#ifndef FMA
	return __t * __b + (_Float{1} - __t) * __a;
#else
	return std::fma(__t, __b, (_Float{1} - __t) * __a);
#endif

    else if (__t == _Float{1}) return __b;

    else { // monotonic near t == 1.
#ifndef FMA
	  const auto __x = __a + __t * (__b - __a);
#else
	  const auto __x = std::fma(__t, __b - __a, __a);
#endif
	    return (__t > _Float{1}) == (__b > __a) ? std::max(__b, __x) : std::min(__b, __x);
	}
}

