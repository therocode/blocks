#pragma once
#include "worldstd.h"

class Region;

class RegionStorageInterface
{
    public:
        virtual bool hasRegion(const RegionCoord& coordinate) = 0;
        virtual const Region& getRegion(const RegionCoord& coordinate) = 0;
};
