#pragma once
#include "worlddata.hpp"
#include "worldparameters.hpp"
#include "../utilities/gridnotifier.hpp"
#include "highlightmanager.hpp"
#include "modmanager.hpp"

class WorldEntry
{
    public:
        WorldEntry(const std::string& identifier);
        void addHighlightEntity(uint32_t id, const ChunkCoord& location);
        void moveHighlightEntity(uint32_t id, const ChunkCoord& location);
        void removeHighlightEntity(uint32_t id);
        const ChunkMap& getChunkMap() const;
        ChunkMap& getChunkMap();
    private:
        void activateChunk(const ChunkCoord& chunk);
        void deactivateChunk(const ChunkCoord& chunk);
        WorldData mWorldData;
        GridNotifier<biomeRegionWidthInChunks> mBiomeGridNotifier;
        HighlightManager mHighlightManager;
        ModManager mModManager; //TBI
};
