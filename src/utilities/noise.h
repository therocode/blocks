#pragma once
#include <stdint.h>
#include <array>
#include <random>
#include <algorithm>

using PermutationTable = std::array<uint8_t, 512>;

class Noise
{
    public:
        Noise();
        Noise(uint32_t seed);
        void setSeed(uint32_t seed);
        float simplex3D(float x, float y, float z) const;
        float simplexOctave3D(float x, float y, float z, float scaleFactor = 1.0f, uint32_t octaves = 5, float persistence = 0.5f) const;
    private:
        std::ranlux24_base mRandomEngine;
        PermutationTable mPerm;
};
