#include "region.h"
#include <iostream>

Region::Region()
{

}

Region::Region(const IntensityMap& heightmap, const IntensityMap& rainmap, const IntensityMap& temperaturemap, const IntensityMap& biomeSelector)
{
    mHeightmap = heightmap;
    mRainmap = rainmap;
    mTemperaturemap = temperaturemap;
    mBiomeSelector = biomeSelector;
}

Region::Region(IntensityMap&& heightmap, IntensityMap&& rainmap, IntensityMap&& temperaturemap, IntensityMap&& biomeSelector)
{
    std::swap(mHeightmap, heightmap);
    std::swap(mRainmap, rainmap);
    std::swap(mTemperaturemap, temperaturemap);
    std::swap(mBiomeSelector, biomeSelector);
}

bool Region::hasChunk(const RegionChunkCoord& location) const
{
    return mChunks.find(location) != mChunks.end();
}

const Chunk& Region::getChunk(const RegionChunkCoord& location) const
{
    return mChunks.at(location);
}

Chunk& Region::getChunk(const RegionChunkCoord& location)
{
    return mChunks.at(location);
}

ChunkReferenceMap Region::getChunkList() const
{
    return ChunkReferenceMap();
}

void Region::addChunk(const RegionChunkCoord& coordinate, const Chunk& chunk)
{
    mChunks.emplace(coordinate, std::move(chunk));
}

void Region::removeChunk(const RegionChunkCoord& coordinate)
{
    mChunks.erase(coordinate);
}

size_t Region::getLoadedChunkAmount() const
{
    return mChunks.size();
}

const IntensityMap& Region::getHeightmap() const
{
    return mHeightmap;
}

const IntensityMap& Region::getRainmap() const
{
    return mRainmap;
}

const IntensityMap& Region::getTemperaturemap() const
{
    return mTemperaturemap;
}

const IntensityMap& Region::getBiomeSelector() const
{
    return mBiomeSelector;
}

void Region::setBiomeTypes(VoxelTypeMap& biomeTypes)
{
    mBiomeTypes = biomeTypes;
}

const VoxelTypeMap& Region::getBiomeTypes() const
{
    return mBiomeTypes;
}
