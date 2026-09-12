#pragma once
#include "Vec2.h"

namespace MF {

    inline static const double HALFPI = 1.570796326794896558;
    inline static const double PI = 3.141592653589793;
    inline static const double TAU = 6.283185307179586;
    inline static const double ROOT2 = 1.4142135623730951;

    inline Vec2 fast_sincos(double angle) {
        return { std::sin(angle), std::cos(angle) };
    }
}