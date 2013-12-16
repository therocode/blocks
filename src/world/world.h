#pragma once
#include "chunk.h"
#include "regionstorageinterface.h"
#include "region.h"

class World : public RegionStorageInterface
{
    public:
        ChunkReferenceMap getChunkMap() const;
        bool hasRegion(const RegionCoordinate& coordinate) override;
        const Region& getRegion(const RegionCoordinate& coordinate) override;
        void addRegion(const RegionCoordinate& coordinate, const Region& region);
    private:
        std::unordered_map<RegionCoordinate, Region> mRegions;
};
