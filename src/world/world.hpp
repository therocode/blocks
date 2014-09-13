#pragma once
#include "chunk.hpp"
#include "region.hpp"
#include <unordered_map>
#include <unordered_set>
#include <fea/util.hpp>

class World
{
    public:
        World(fea::MessageBus& b);
        ChunkReferenceMap getChunkMap() const;
        bool hasRegion(const RegionCoord& coordinate) const;
        const Region& getRegion(const RegionCoord& coordinate) const;
        void addRegion(const RegionCoord& coordinate, const Region& region);
        void addChunk(const ChunkCoord& coordinate, const Chunk& chunk);
        void removeChunk(const ChunkCoord& coordinate);
        void removeRegion(const RegionCoord& coordinate);
        VoxelType getVoxelType(const VoxelCoord& voxelCoordinate) const;
        bool setVoxelType(const VoxelCoord& voxelCoord, VoxelType type);
        void activateChunk(const ChunkCoord& coordinate);
        void deactivateChunk(const ChunkCoord& coordinate);
    private:
        fea::MessageBus& mBus;
        std::unordered_map<RegionCoord, Region> mRegions;

        std::unordered_map<RegionCoord, std::unordered_set<ChunkCoord>> mActiveRegions;
};
