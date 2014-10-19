#pragma once
#include <unordered_map>
#include <vector>
#include <fea/util.hpp>
#include "worlddata.hpp"
#include "worldparameters.hpp"
#include "../utilities/gridnotifier.hpp"
#include "highlightmanager.hpp"
#include "modmanager.hpp"

class WorldEntry
{
    public:
        WorldEntry(fea::MessageBus& bus, WorldId id, const std::string& identifier, const WorldData& data, const std::string& path);
        ~WorldEntry();
        void addHighlightEntity(uint32_t id, const ChunkCoord& location);
        void moveHighlightEntity(uint32_t id, const ChunkCoord& location);
        void removeHighlightEntity(uint32_t id);
        void deliverBiome(const BiomeRegionCoord& coordinate, const BiomeGrid& biomeData);
        void deliverChunk(const ChunkCoord& coordinate, const Chunk& chunk);
        const ChunkMap& getChunkMap() const;
        ChunkMap& getChunkMap();
        void setVoxelType(const VoxelCoord& voxelCoordinate, VoxelType type);
        void chunksRequested(const std::vector<ChunkCoord>& coordinates);
        const std::string& getIdentifier() const;
    private:
        void activateChunk(const ChunkCoord& chunk);
        void deactivateChunk(const ChunkCoord& chunk);
        void requestChunk(const ChunkCoord& chunk);
        fea::MessageBus& mBus;
        WorldId mId;
        std::string mIdentifier;
        WorldData mWorldData;
        GridNotifier<biomeRegionWidthInChunks> mBiomeGridNotifier;
        HighlightManager mHighlightManager;
        ModManager mModManager;

        std::unordered_map<BiomeRegionCoord, std::vector<ChunkCoord>> mPendingChunksToRequests;
};
