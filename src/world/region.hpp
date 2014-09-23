#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include "chunk.hpp"
#include "valuemap.hpp"
#include "regiondatafragment.hpp"

class Biome;

using ChunkRegionMap = std::unordered_map<RegionChunkCoord, Chunk>;

class Region
{
    public:
        Region();
        Region(const FloatMap& heightmap, const FloatMap& rainmap, const FloatMap& temperaturemap, const FloatMap& biomeSelector);
        Region(FloatMap&& heightmap, FloatMap&& rainmap, FloatMap&& temperaturemap, FloatMap&& biomeSelector);
        bool hasChunk(const RegionChunkCoord& location) const;
        const Chunk& getChunk(const RegionChunkCoord& location) const;
        Chunk& getChunk(const RegionChunkCoord& location);
        ChunkReferenceMap getChunkList() const;  //implement this for real. it just returns ChunkReferenceMap() right now!
        void addChunk(const RegionChunkCoord& coordinate, const Chunk& chunk);
        void removeChunk(const RegionChunkCoord& coordinate);
        size_t getLoadedChunkAmount() const;
        const FloatMap& getHeightmap() const;
        const FloatMap& getRainmap() const;
        const FloatMap& getTemperaturemap() const;
        const FloatMap& getBiomeSelector() const;
        void setBiomes(const FloatMap& biomeIndices);
        //const Biome* getBiome(const RegionVoxelCoord& coordinate) const;
        RegionDataFragment getDataFragment(const glm::uvec2& start, const glm::uvec2& end);
    private:
        FloatMap getMapFragment(const glm::uvec2& start, const glm::uvec2& end, const FloatMap& map);
        ChunkRegionMap mChunks;
        FloatMap mHeightmap;
        FloatMap mRainmap;
        FloatMap mTemperaturemap;
        FloatMap mBiomeSelector;
};
