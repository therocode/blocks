#include "region.hpp"
#include <iostream>

Region::Region() :
    mHeightmap(regionVoxelWidth, regionVoxelWidth),
    mRainmap(regionVoxelWidth, regionVoxelWidth),
    mTemperaturemap(regionVoxelWidth, regionVoxelWidth),
    mBiomeSelector(regionVoxelWidth, regionVoxelWidth),
    mBiomeIndices(regionVoxelWidth, regionVoxelWidth)
{

}

Region::Region(const FloatMap& heightmap, const FloatMap& rainmap, const FloatMap& temperaturemap, const FloatMap& biomeSelector) :
    mHeightmap(heightmap),
    mRainmap(rainmap),
    mTemperaturemap(temperaturemap),
    mBiomeSelector(biomeSelector),
    mBiomeIndices(regionVoxelWidth, regionVoxelWidth)
{
}

Region::Region(FloatMap&& heightmap, FloatMap&& rainmap, FloatMap&& temperaturemap, FloatMap&& biomeSelector) :
    mHeightmap(std::move(heightmap)),
    mRainmap(std::move(rainmap)),
    mTemperaturemap(std::move(temperaturemap)),
    mBiomeSelector(std::move(biomeSelector)),
    mBiomeIndices(regionVoxelWidth, regionVoxelWidth)
{
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

const FloatMap& Region::getHeightmap() const
{
    return mHeightmap;
}

const FloatMap& Region::getRainmap() const
{
    return mRainmap;
}

const FloatMap& Region::getTemperaturemap() const
{
    return mTemperaturemap;
}

const FloatMap& Region::getBiomeSelector() const
{
    return mBiomeSelector;
}

void Region::setBiomes(const FloatMap& biomeIndices)
{
    mBiomeIndices = biomeIndices;
}

//const Biome* Region::getBiome(const RegionVoxelCoord& coordinate) const
//{
//    return mBiomeMap.at(mBiomeIndices.getUnit(coordinate.x, coordinate.y));
//}

RegionDataFragment Region::getDataFragment(const glm::uvec2& start, const glm::uvec2& end)
{
}
