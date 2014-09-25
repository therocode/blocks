#pragma once
#include "../valuemap.hpp"

using RegionCoord = glm::i64vec2;

class RegionDataGenerator
{
    public:
        void setSeed(int32_t seed);
        FloatMap generateHeightmap(const RegionCoord& regionCoordinate) const;
        FloatMap generateRainfall(const RegionCoord& regionCoordinate) const;
        FloatMap generateTemperature(const RegionCoord& regionCoordinate, const FloatMap& height) const;
        FloatMap generateBiomeSelector(const RegionCoord& regionCoordinate) const;
    private:
        int32_t mSeed;
};
