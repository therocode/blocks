#pragma once
#include "chunk.hpp"
#include "regionstorageinterface.hpp"
#include "region.hpp"

class World : public RegionStorageInterface
{
    public:
        ChunkReferenceMap getChunkMap() const;
        bool hasRegion(const RegionCoord& coordinate) const override;
        const Region& getRegion(const RegionCoord& coordinate) const override;
        void addRegion(const RegionCoord& coordinate, const Region& region);
        void addChunk(const ChunkCoord& coordinate, const Chunk& chunk);
        bool removeChunk(const ChunkCoord& coordinate);
        VoxelType getVoxelType(const VoxelCoord& voxelCoordinate) const;
        bool setVoxelType(const VoxelCoord& voxelCoord, VoxelType type);
    private:
        std::unordered_map<RegionCoord, Region> mRegions;
};
