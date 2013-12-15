#pragma once
#include "chunk.h"
#include "regionproviderinterface.h"
#include "region.h"

class World : public RegionProviderInterface
{
    public:
        ChunkReferenceMap getChunkMap() const;
        bool hasRegion(const RegionCoordinate& coordinate) override;
        const Region& getRegion(const RegionCoordinate& coordinate) override;
    private:
        std::unordered_map<RegionCoordinate, Region> mRegions;
};
