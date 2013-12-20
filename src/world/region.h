#pragma once
#include "chunkprovider.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include "chunk.h"

using ChunkRegionMap = std::unordered_map<RegionChunkCoord, Chunk>;

class Region
{
    public:
        bool chunkIsLoaded(const RegionChunkCoord& location) const;
        const Chunk& getChunk(const RegionChunkCoord& location) const;
        Chunk& getChunk(const RegionChunkCoord& location);
        ChunkReferenceMap getChunkList() const;
        void addChunk(const RegionChunkCoord& coordinate, const Chunk& chunk);
    private:
        ChunkRegionMap chunks;
};
