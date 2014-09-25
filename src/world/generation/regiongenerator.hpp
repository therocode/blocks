#pragma once
#include <fea/util.hpp>
#include "regiondatagenerator.hpp"
#include "../region.hpp"

using RegionCoord = glm::i64vec2;

struct Pixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

class RegionGenerator
{
    public:
        RegionGenerator();
        Region generateRegion(const RegionCoord& coordinate);
        void setSeed(int32_t seed);
    private:
        RegionDataGenerator mRegionDataGenerator;
};
