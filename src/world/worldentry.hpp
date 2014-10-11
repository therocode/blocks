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
        WorldEntry(fea::MessageBus& bus, WorldId id, const std::string& identifier, const WorldData& data);
        void addHighlightEntity(uint32_t id, const ChunkCoord& location);
        void moveHighlightEntity(uint32_t id, const ChunkCoord& location);
        void removeHighlightEntity(uint32_t id);
        void deliverBiome(const BiomeRegionCoord& coordinate, const BiomeGrid& biomeData);
        const ChunkMap& getChunkMap() const;
        ChunkMap& getChunkMap();
    private:
        void activateChunk(const ChunkCoord& chunk);
        void deactivateChunk(const ChunkCoord& chunk);
        fea::MessageBus& mBus;
        WorldId mId;
        WorldData mWorldData;
        GridNotifier<biomeRegionWidthInChunks> mBiomeGridNotifier;
        HighlightManager mHighlightManager;
        ModManager mModManager; //TBI

        std::unordered_map<BiomeRegionCoord, std::vector<ChunkCoord>> mPendingChunksToRequests;
};
