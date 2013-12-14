#pragma once
#include "chunk.h"
#include "region.h"
#include <featherkit/messaging.h>

class World
{
    public:
        World(fea::MessageBus& messageBus);
        void initialise(); 
        void highlightChunk(size_t id, const ChunkCoordinate& chunk);
        ChunkReferenceMap getChunkMap() const;
    private:
        Region loadRegion(const RegionCoordinate& coordinate);
        fea::MessageBus& bus;
        std::unordered_map<RegionCoordinate, Region> mRegions;
        std::unordered_map<size_t, ChunkCoordinate> highlights;
};
