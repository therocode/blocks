#pragma once
#include "../valuemap.h"
#include "../worldstd.h"
#include "../region.h"

class RegionDataGenerator
{
    public:
        IntensityMap generateHeightmap(const RegionCoord& regionCoordinate) const;
        IntensityMap generateRainfall(const RegionCoord& regionCoordinate) const;
        IntensityMap generateTemperature(const RegionCoord& regionCoordinate, const IntensityMap& height) const;
        IntensityMap generateBiomeSelector(const RegionCoord& regionCoordinate) const;
};
