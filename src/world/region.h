#pragma once
#include "chunkprovider.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <featherkit/messaging.h>

using ChunkRegionMap = std::unordered_map<ChunkRegionCoordinate, Chunk>;

class Region
{
    public:
        Region(fea::MessageBus& bus);
        void setChunkProvider(std::unique_ptr<ChunkProvider> del);
        Chunk& loadChunk(const ChunkRegionCoordinate& location);
        bool chunkIsLoaded(const ChunkRegionCoordinate& location) const;
        const Chunk& getChunk(const ChunkRegionCoordinate& location) const;
        Chunk& getChunk(const ChunkRegionCoordinate& location);
        ChunkReferenceMap getChunkList() const;
        void highlightChunk(size_t id, const ChunkRegionCoordinate& chunk);
    private:
        void checkUnloads(size_t id);
        void unloadChunk(const ChunkRegionCoordinate& chunk);
        fea::MessageBus& mBus;
        ChunkRegionMap chunks;
        std::unique_ptr<ChunkProvider> chunkDeliverer;
        std::unordered_map<size_t, ChunkRegionCoordinate> highlightedChunks;
};
