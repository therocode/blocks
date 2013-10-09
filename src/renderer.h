#pragma once
#include "chunk.h"
#include "chunkvbo.h"
#include <vector>

class Renderer
{
    public:
        void setup();
        void addChunk(const Chunk& chunk);
        void render();
        void hehehe();
    private:
        std::vector<ChunkVBO> chunks;
        float hehe = 0.0f;
};
