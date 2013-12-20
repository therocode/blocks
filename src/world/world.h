#pragma once
#include "chunk.h"
#include "regionstorageinterface.h"
#include "region.h"

class World : public RegionStorageInterface
{
    public:
        ChunkReferenceMap getChunkMap() const;
        bool hasRegion(const RegionCoord& coordinate) override;
        const Region& getRegion(const RegionCoord& coordinate) override;
        void addRegion(const RegionCoord& coordinate, const Region& region);
        void addChunk(const ChunkCoord& coordinate, const Chunk& chunk);
        VoxelType getVoxelType(const VoxelCoord& voxelCoord);
    private:
        std::unordered_map<RegionCoord, Region> mRegions;
};
