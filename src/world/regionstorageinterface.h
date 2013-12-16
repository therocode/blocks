#pragma once
#include "worldstd.h"

class Region;

class RegionStorageInterface
{
    public:
        virtual bool hasRegion(const RegionCoordinate& coordinate) = 0;
        virtual const Region& getRegion(const RegionCoordinate& coordinate) = 0;
};
