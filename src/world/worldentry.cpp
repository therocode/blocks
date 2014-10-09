#include "worldentry.hpp"

WorldEntry::WorldEntry(const std::string& identifier) :
    mHighlightManager(8), //this should be read from somewhere else, probably server settings
    mModManager(identifier)
{
}

void WorldEntry::addHighlightEntity(uint32_t id, const ChunkCoord& location)
{
    const auto& highlighted = mHighlightManager.addHighlightEntity(id, location);

    for(const auto& chunk : highlighted)
        activateChunk(chunk);
}

void WorldEntry::moveHighlightEntity(uint32_t id, const ChunkCoord& location)
{
    const auto& highlightedDehighlighted = mHighlightManager.moveHighlightEntity(id, location);

    for(const auto& chunk : highlightedDehighlighted.first)
        activateChunk(chunk);

    for(const auto& chunk : highlightedDehighlighted.second)
        deactivateChunk(chunk);
}


void WorldEntry::removeHighlightEntity(uint32_t id)
{
    const auto& dehighlighted = mHighlightManager.removeHighlightEntity(id);

    for(const auto& chunk : dehighlighted)
        deactivateChunk(chunk);
}

const ChunkMap& WorldEntry::getChunkMap() const
{
    return mWorldData.voxels;
}

ChunkMap& WorldEntry::getChunkMap()
{
    return mWorldData.voxels;
}

void WorldEntry::activateChunk(const ChunkCoord& chunkCoordinate)
{
    //firsty handle any newly activated biomes; they need to be requested
    const auto& activatedBiomes = mBiomeGridNotifier.set(chunkCoordinate, true);

    for(const auto& biomeRegionCoord : activatedBiomes)
    {
        //send biome request asynchronously
    }

    //secondly, the activated chunk will need to be generated as well, but we can't do that if the biome hasn't been delivered since we need data from the biome to pass on to the generator. Hence, if the biome doesn't exist, we store the request
    const BiomeRegionCoord biomeRegionCoord = ChunkToBiomeRegion::convert(chunkCoordinate);
    auto iterator = mWorldData.biomeGrids.find(biomeRegionCoord);
    if(iterator != mWorldData.biomeGrids.end())
    {
        //send chunk request with biome data
    }
    else
    {
        mPendingChunksToRequests[biomeRegionCoord].push_back(chunkCoordinate);
    }
}

void WorldEntry::deactivateChunk(const ChunkCoord& chunkCoordinate)
{
}
