#include "region.hpp"
#include <iostream>

Region::Region() :
    mHeightmap(regionVoxelWidth, regionVoxelWidth),
    mRainmap(regionVoxelWidth, regionVoxelWidth),
    mTemperaturemap(regionVoxelWidth, regionVoxelWidth),
    mBiomeSelector(regionVoxelWidth, regionVoxelWidth)
{

}

Region::Region(const FloatMap& heightmap, const FloatMap& rainmap, const FloatMap& temperaturemap, const FloatMap& biomeSelector) :
    mHeightmap(heightmap),
    mRainmap(rainmap),
    mTemperaturemap(temperaturemap),
    mBiomeSelector(biomeSelector)
{
}

Region::Region(FloatMap&& heightmap, FloatMap&& rainmap, FloatMap&& temperaturemap, FloatMap&& biomeSelector) :
    mHeightmap(std::move(heightmap)),
    mRainmap(std::move(rainmap)),
    mTemperaturemap(std::move(temperaturemap)),
    mBiomeSelector(std::move(biomeSelector))
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

const ChunkRegionMap& Region::getChunkList() const
{
    return mChunks;
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

RegionDataFragment Region::getDataFragment(const glm::uvec2& start, const glm::uvec2& end) const
{
    return RegionDataFragment(getMapFragment(start, end, mHeightmap),
                              getMapFragment(start, end, mTemperaturemap),
                              getMapFragment(start, end, mRainmap),
                              getMapFragment(start, end, mBiomeSelector));
}

FloatMap Region::getMapFragment(const glm::uvec2& start, const glm::uvec2& end, const FloatMap& map) const
{
    FloatMap result(end.x, end.y);

    for(uint32_t y = 0; y < end.y; y++)
    {
        for(uint32_t x = 0; x < end.x; x++)
        {
            result.setUnit(x, y, map.getUnit(x + start.x, y + start.y));
        }
    }

    return result;
}
