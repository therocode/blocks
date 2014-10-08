#pragma once
#include <fea/util.hpp>
#include "regiondatagenerator.hpp"
#include "../region.hpp"

class RegionGenerator
{
    public:
        RegionGenerator();
        Region generateRegion(const BiomeRegionCoord& coordinate);
        void setSeed(int32_t seed);
    private:
        RegionDataGenerator mRegionDataGenerator;
};
