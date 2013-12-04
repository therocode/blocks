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
        const Region& getLandscape() const;
        Region& getLandscape();
    private:
        fea::MessageBus& bus;
        Region landscape;
        std::unordered_map<size_t, ChunkCoordinate> highlights;
};
