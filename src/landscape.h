#pragma once
#include "chunkprovider.h"
#include <unordered_map>
#include <vector>

class Landscape
{
    public:
        void setChunkProvider(ChunkProvider* del);
        Chunk& loadChunk(const ChunkCoordinate& location);
        bool chunkIsLoaded(const ChunkCoordinate& location) const;
        const Chunk& getChunk(const ChunkCoordinate& location) const;
    private:
        std::unordered_map<ChunkCoordinate, uint32_t> chunkIndices;
        std::vector<Chunk> chunks;
        ChunkProvider* chunkDeliverer; //unique_ptr
};
