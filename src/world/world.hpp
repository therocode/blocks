#pragma once
#include "chunk.hpp"
#include "region.hpp"
#include "highlightmanager.hpp"
#include "modmanager.hpp"
#include <unordered_map>
#include <unordered_set>
#include <fea/util.hpp>

using IntRange = std::pair<int32_t, int32_t>;

struct Ranges
{
    static int32_t MAX;
    static int32_t MIN;
    Ranges(const IntRange& x, const IntRange& y, const IntRange& z);
    bool isWithin(const glm::ivec3& coordinate) const;
    IntRange xRange;
    IntRange yRange;
    IntRange zRange;
};

class World
{
    public:
        World(fea::MessageBus& b, WorldId id, const std::string& textIdentifier, const Ranges& ranges);
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
        void removeChunk(const ChunkCoord& coordinate);

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
