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
        bool chunkIsLoaded(const ChunkRegionCoordinate& location) const;
        const Chunk& getChunk(const ChunkRegionCoordinate& location) const;
        Chunk& getChunk(const ChunkRegionCoordinate& location);
        ChunkReferenceMap getChunkList() const;
        void addChunk(const ChunkRegionCoordinate& coordinate, const Chunk& chunk);
    private:
        ChunkRegionMap chunks;
};
