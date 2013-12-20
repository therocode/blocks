#pragma once
#include "worldstd.h"

class Region;

class RegionStorageInterface
{
    public:
        virtual bool hasRegion(const RegionCoord& coordinate) const = 0;
        virtual const Region& getRegion(const RegionCoord& coordinate) const = 0;
};
