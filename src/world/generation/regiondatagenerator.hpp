#pragma once
#include "../valuemap.hpp"
#include "../worldstd.hpp"
#include "../region.hpp"

class RegionDataGenerator
{
    public:
        void setSeed(int32_t seed);
        IntensityMap generateHeightmap(const RegionCoord& regionCoordinate) const;
        IntensityMap generateRainfall(const RegionCoord& regionCoordinate) const;
        IntensityMap generateTemperature(const RegionCoord& regionCoordinate, const IntensityMap& height) const;
        IntensityMap generateBiomeSelector(const RegionCoord& regionCoordinate) const;
    private:
        int32_t mSeed;
};
