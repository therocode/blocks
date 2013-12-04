#pragma once
#include "chunk.h"
#include "landscape.h"
#include <featherkit/messaging.h>

class World
{
    public:
        World(fea::MessageBus& messageBus);
        void initialise(); 
        void highlightChunk(size_t id, const ChunkCoordinate& chunk);
        const Landscape& getLandscape() const;
        Landscape& getLandscape();
    private:
        fea::MessageBus& bus;
        Landscape landscape;
        std::unordered_map<size_t, ChunkCoordinate> highlights;
};
