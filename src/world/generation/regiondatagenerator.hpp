#pragma once
#include "../valuemap.hpp"

using BiomeRegionCoord = glm::i64vec2;

class RegionDataGenerator
{
    public:
        void setSeed(int32_t seed);
        FloatMap generateHeightmap(const BiomeRegionCoord& regionCoordinate) const;
        FloatMap generateRainfall(const BiomeRegionCoord& regionCoordinate) const;
        FloatMap generateTemperature(const BiomeRegionCoord& regionCoordinate, const FloatMap& height) const;
        FloatMap generateBiomeSelector(const BiomeRegionCoord& regionCoordinate) const;
    private:
        int32_t mSeed;
};
