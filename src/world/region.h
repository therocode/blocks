#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include "chunk.h"
#include "valuemap.h"

class Biome;

using ChunkRegionMap = std::unordered_map<RegionChunkCoord, Chunk>;
using IntensityMap = ValueMap<float>;
using BiomeIndices = ValueMap<uint16_t>;
using BiomeIndexMap = std::unordered_map<uint16_t, Biome*>;

class Region
{
    public:
        Region();
        Region(const IntensityMap& heightmap, const IntensityMap& rainmap, const IntensityMap& temperaturemap, const IntensityMap& biomeSelector);
        Region(IntensityMap&& heightmap, IntensityMap&& rainmap, IntensityMap&& temperaturemap, IntensityMap&& biomeSelector);
        bool hasChunk(const RegionChunkCoord& location) const;
        const Chunk& getChunk(const RegionChunkCoord& location) const;
        Chunk& getChunk(const RegionChunkCoord& location);
        ChunkReferenceMap getChunkList() const;
        void addChunk(const RegionChunkCoord& coordinate, const Chunk& chunk);
        void removeChunk(const RegionChunkCoord& coordinate);
        size_t getLoadedChunkAmount() const;
        const IntensityMap& getHeightmap() const;
        const IntensityMap& getRainmap() const;
        const IntensityMap& getTemperaturemap() const;
        const IntensityMap& getBiomeSelector() const;
        void setBiomes(const BiomeIndices& biomeIndices, const BiomeIndexMap& biomeMap);
        const Biome* getBiome(const RegionVoxelCoord& coordinate) const;
    private:
        ChunkRegionMap mChunks;
        IntensityMap mHeightmap;
        IntensityMap mRainmap;
        IntensityMap mTemperaturemap;
        IntensityMap mBiomeSelector;
        BiomeIndices mBiomeIndices;
        BiomeIndexMap mBiomeMap;
};
