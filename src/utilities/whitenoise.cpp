#include "whitenoise.h"
#include <iostream>
#include <cmath>

float WhiteNoise::get2d(const float x, const float y, const uint8_t* perm) const
{
    int i = std::floor(x);
    int j = std::floor(y);

    // Work out the hashed gradient indices of the three simplex corners
    int16_t ii = i & 255;
    int16_t jj = j & 255;
    int gi0 = perm[ii + perm[jj]];

    //std::cout << "gi0 is now " << gi0 << "\n";

    // The result is scaled to return values in the interval [-1,1].
    return ((float)gi0 / 128.0f) - 1.0f;
}
