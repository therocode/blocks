#pragma once
#include "../intensitymap.h"
#include "../worldstd.h"

class RegionGenerator
{
    public:
        IntensityMap generateHeightmap(const RegionCoord& regionCoordinate) const;
};
