#pragma once
#include "chunk.h"
#include <unordered_map>
#include <vector>
#include <featherkit/messaging.h>

class Dimension
{
    public:
        Dimension(fea::MessageBus& messageBus);
        void initialise(); 
    private:
        fea::MessageBus& bus;
        std::unordered_map<ChunkCoordinate, uint32_t> chunkIndices;
        std::vector<Chunk> chunks;
};
