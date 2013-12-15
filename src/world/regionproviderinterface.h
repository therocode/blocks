#pragma once
#include "worldstd.h"
#include "region.h"

class RegionProviderInterface
{
    public:
        virtual bool hasRegion(const RegionCoordinate& coordinate) = 0;
        virtual const Region& getRegion(const RegionCoordinate& coordinate) = 0;
};
