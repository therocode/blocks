#pragma once
#include "chunk.hpp"
#include "region.hpp"
#include "highlightmanager.hpp"
#include "modmanager.hpp"
#include <unordered_map>
#include <unordered_set>
#include <fea/util.hpp>
#include "ranges.hpp"

class World
{
    public:
        World(fea::MessageBus& b, WorldId id, const std::string& textIdentifier, const std::string& title, const Ranges& ranges);
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
        void activateChunk(const ChunkCoord& coordinate, int32_t priority);
        void deactivateChunk(const ChunkCoord& coordinate);

        fea::MessageBus& mBus;

        //world information
        WorldId     mId;
        std::string mTextIdentifier;
        std::string mTitle;  //unused right now

        Ranges mWorldRange;

        std::unordered_map<RegionCoord, Region> mRegions;

        //highlight management
        HighlightManager mHighlightManager;
        ModManager mModManager;
        std::unordered_map<RegionCoord, std::unordered_set<ChunkCoord>> mHighlightedRegions;
};
