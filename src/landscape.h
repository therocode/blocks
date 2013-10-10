#pragma once
#include "chunkdeliverer.h"
#include <unordered_map>
#include <vector>

class Landscape
{
    public:
        void setChunkDeliverer(ChunkDeliverer* del);
        Chunk& loadChunk(const ChunkCoordinate& location);
    private:
        std::unordered_map<ChunkCoordinate, uint32_t> chunkIndices;
        std::vector<Chunk> chunks;
        ChunkDeliverer* chunkDeliverer; //unique_ptr
};
