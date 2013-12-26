#pragma once
#include "chunkprovider.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include "chunk.h"
#include "valuemap.h"

using ChunkRegionMap = std::unordered_map<RegionChunkCoord, Chunk>;
using IntensityMap = ValueMap<float>;
using VoxelTypeMap = ValueMap<VoxelType>;

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
        void setBiomeTypes(VoxelTypeMap& biomeTypes);
        const VoxelTypeMap& getBiomeTypes() const;
    private:
        ChunkRegionMap mChunks;
        IntensityMap mHeightmap;
        IntensityMap mRainmap;
        IntensityMap mTemperaturemap;
        IntensityMap mBiomeSelector;
        VoxelTypeMap mBiomeTypes;
};
