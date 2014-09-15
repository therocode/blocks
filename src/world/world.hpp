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
        void deliverRegion(const RegionCoord& coordinate, const Region& region);
        void deliverChunk(const ChunkCoord& coordinate, Chunk& chunk);
        VoxelType getVoxelType(const VoxelCoord& voxelCoordinate) const;
        bool setVoxelType(const VoxelCoord& voxelCoord, VoxelType type);

        void addHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate);
        void removeHighlightEntity(fea::EntityId id);
        void moveHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate);
    private:
        bool hasRegion(const RegionCoord& coordinate) const;
        void removeChunk(const ChunkCoord& coordinate);
        void removeRegion(const RegionCoord& coordinate);
        void activateChunk(const ChunkCoord& coordinate);
        void deactivateChunk(const ChunkCoord& coordinate);

        fea::MessageBus& mBus;

        //world information
        std::string mIdentifier;
        std::string mTitle;  //unused right now

        std::unordered_map<RegionCoord, Region> mRegions;

        //highlight management
        HighlightManager mHighlightManager;
        ModManager mModManager;
        std::unordered_map<RegionCoord, std::unordered_set<ChunkCoord>> mHighlightedRegions;
};
