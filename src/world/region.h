#pragma once
#include "chunkprovider.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include "chunk.h"
#include "intensitymap.h"

using ChunkRegionMap = std::unordered_map<RegionChunkCoord, Chunk>;

class Region
{
    public:
        Region();
        Region(const IntensityMap& heightmap);
        Region(IntensityMap&& heightmap);
        bool hasChunk(const RegionChunkCoord& location) const;
        const Chunk& getChunk(const RegionChunkCoord& location) const;
        Chunk& getChunk(const RegionChunkCoord& location);
        ChunkReferenceMap getChunkList() const;
        void addChunk(const RegionChunkCoord& coordinate, const Chunk& chunk);
        const IntensityMap& getHeightmap() const;
    private:
        ChunkRegionMap mChunks;
        IntensityMap mHeightmap;
};
