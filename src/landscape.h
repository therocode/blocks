#pragma once
#include "chunkprovider.h"
#include <unordered_map>
#include <vector>
#include <memory>

class Landscape
{
    public:
        void setChunkProvider(std::unique_ptr<ChunkProvider> del);
        Chunk& loadChunk(const ChunkCoordinate& location);
        bool chunkIsLoaded(const ChunkCoordinate& location) const;
        const Chunk& getChunk(const ChunkCoordinate& location) const;
    private:
        std::unordered_map<ChunkCoordinate, uint32_t> chunkIndices;
        std::vector<Chunk> chunks;
        std::unique_ptr<ChunkProvider> chunkDeliverer;
};
