#pragma once
#include <stdint.h>

class VoronoiNoise
{
    public:
        float get2d(float x, float y, const uint8_t* perm) const;
};
