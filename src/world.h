#pragma once
#include "chunk.h"
#include <unordered_map>
#include <vector>
#include "renderer.h"

class World
{
    public:
        void initialise();
        void update();
    private:
        std::unordered_map<ChunkCoordinate, uint32_t> chunkIndices;
        std::vector<Chunk> chunks;
        Renderer renderer;  //this shouldn't be here in the future
};
