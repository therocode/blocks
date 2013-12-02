#pragma once
#include <stdint.h>
#include <array>
#include <random>
#include <algorithm>

class Noise
{
    public:
        Noise();
        Noise(uint32_t seed);
        void setSeed(uint32_t seed);
    private:
        std::ranlux24_base mRandomEngine;
        std::array<uint8_t, 512> mPerm;
};
