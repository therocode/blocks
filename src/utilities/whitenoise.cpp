#include "whitenoise.h"
#include <iostream>
#include <cmath>

float WhiteNoise::get2d(const float x, const float y, const uint8_t* perm) const
{
    int32_t i = (int32_t)std::floor(x) % 255;
    int32_t j = (int32_t)std::floor(y) % 255;

    uint32_t res = perm[i + perm[j]];

    return (float) res / 255.0f;
}
