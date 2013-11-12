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
        Chunk& getChunk(const ChunkCoordinate& location);
        const ChunkMap& getChunkList() const;
        void highlightChunk(size_t id, const ChunkCoordinate& chunk);
    private:
        void checkUnloads(size_t id);
        void unloadChunk(const ChunkCoordinate& chunk);
        fea::MessageBus& mBus;
        ChunkMap chunks;
        std::unique_ptr<ChunkProvider> chunkDeliverer;
        std::unordered_map<size_t, ChunkCoordinate> highlightedChunks;


        bool tempful = false; //away scumbag
};
