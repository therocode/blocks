#pragma once
#include "chunkprovider.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include "chunk.h"

using ChunkRegionMap = std::unordered_map<ChunkRegionCoordinate, Chunk>;

class Region
{
    public:
        Chunk& loadChunk(const ChunkRegionCoordinate& location);
        bool chunkIsLoaded(const ChunkRegionCoordinate& location) const;
        const Chunk& getChunk(const ChunkRegionCoordinate& location) const;
        Chunk& getChunk(const ChunkRegionCoordinate& location);
        ChunkReferenceMap getChunkList() const;
        void highlightChunk(size_t id, const ChunkRegionCoordinate& chunk);
    private:
        void checkUnloads(size_t id);
        void unloadChunk(const ChunkRegionCoordinate& chunk);
        ChunkRegionMap chunks;
        std::unordered_map<size_t, ChunkRegionCoordinate> highlightedChunks;
};
