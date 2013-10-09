#pragma once
#include "chunk.h"
#include <unordered_map>
#include <vector>
#include "renderer.h"

class World
{
    public:
        World(fea::MessageBus& messageBus);
        void initialise();
        void update();
        void hehehe();
    private:
        std::unordered_map<ChunkCoordinate, uint32_t> chunkIndices;
        std::vector<Chunk> chunks;
        fea::MessageBus& bus;
};
