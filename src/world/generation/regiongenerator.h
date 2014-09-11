#pragma once
#include <fea/util.hpp>
#include "regiondatagenerator.h"
#include "../biomestorage.h"

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
    private:
        RegionDataGenerator mRegionDataGenerator;
        BiomeStorage mStorage;
        std::array<Pixel, regionVoxelWidthx2> mImage;
        std::array<Pixel, regionVoxelWidthx2> mRain;
        std::array<Pixel, regionVoxelWidthx2> mTemp;
        std::array<Pixel, regionVoxelWidthx2> mHeight;
};
