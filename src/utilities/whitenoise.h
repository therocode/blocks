#pragma once
#include <stdint.h>

class WhiteNoise
{
    public:
        float get2d(const float x, const float y, const uint8_t* perm) const;
    private:
        uint32_t joinBits(uint16_t a, uint16_t b) const;
};
