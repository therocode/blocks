#pragma once
#include "chunkprovider.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <featherkit/messaging.h>

class Landscape
{
    public:
        Landscape(fea::MessageBus& bus);
        void setChunkProvider(std::unique_ptr<ChunkProvider> del);
        Chunk& loadChunk(const ChunkCoordinate& location);
        bool chunkIsLoaded(const ChunkCoordinate& location) const;
        const Chunk& getChunk(const ChunkCoordinate& location) const;
        const std::vector<Chunk>& getChunkList() const;
        void highlightChunk(size_t id, const ChunkCoordinate& chunk);
    private:
        void checkUnloads(size_t id);
        fea::MessageBus& mBus;
        std::unordered_map<ChunkCoordinate, uint32_t> chunkIndices;
        std::vector<Chunk> chunks;
        std::unique_ptr<ChunkProvider> chunkDeliverer;
};
