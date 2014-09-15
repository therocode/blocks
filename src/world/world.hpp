#pragma once
#include "chunk.hpp"
#include "region.hpp"
#include "highlightmanager.hpp"
#include "modmanager.hpp"
#include <unordered_map>
#include <unordered_set>
#include <fea/util.hpp>

class World
{
    public:
        World(fea::MessageBus& b, const std::string& identifier);
        ~World();
        ChunkReferenceMap getChunkMap() const;
        bool hasRegion(const RegionCoord& coordinate) const;
        const Region& getRegion(const RegionCoord& coordinate) const;
        void addRegion(const RegionCoord& coordinate, const Region& region);
        void addChunk(const ChunkCoord& coordinate, Chunk& chunk);
        void removeChunk(const ChunkCoord& coordinate);
        void removeRegion(const RegionCoord& coordinate);
        VoxelType getVoxelType(const VoxelCoord& voxelCoordinate) const;
        bool setVoxelType(const VoxelCoord& voxelCoord, VoxelType type);
        void activateChunk(const ChunkCoord& coordinate);
        void deactivateChunk(const ChunkCoord& coordinate);

        void addHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate);
        void removeHighlightEntity(fea::EntityId id);
        void moveHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate);
    private:
        fea::MessageBus& mBus;

        std::string mIdentifier;

        std::unordered_map<RegionCoord, Region> mRegions;
        std::unordered_map<RegionCoord, std::unordered_set<ChunkCoord>> mActiveRegions;

        HighlightManager mHighlightManager;
        ModManager mModManager;
};
