#include "worldentry.hpp"
#include "worldmessages.hpp"
#include <algorithm>

WorldEntry::WorldEntry(fea::MessageBus& bus, WorldId id, const std::string& identifier, const WorldData& data) :
    mBus(bus),
    mId(id),
    mWorldData(data),
    mHighlightManager(8), //this should be read from somewhere else, probably server settings
    mModManager(identifier)
{
    mWorldData.biomeSettings.fields =
    {
        Field{"height"},
        Field{"rainfall"},
        Field{"temperature"},
        Field{"selector"}
    };

    mBus.send(WorldBiomeSettingsMessage{id, mWorldData.biomeSettings});
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

void WorldEntry::deliverBiome(const BiomeRegionCoord& coordinate, const BiomeGrid& biomeData)
{
    if(mBiomeGridNotifier.isActive(coordinate))
    {
        mWorldData.biomeGrids.emplace(coordinate, biomeData);

        auto iterator = mPendingChunksToRequests.find(coordinate);

        if(iterator != mPendingChunksToRequests.end())
        {
            for(const auto& chunk : iterator->second)
                ;//send chunk request with biome data
        }
    }
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
        mBus.send(BiomeRequestedMessage{mId, biomeRegionCoord});
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
    //first lets see if this also deactivates a biome. in that case, it must be removed
    const auto& deactivatedBiomes = mBiomeGridNotifier.set(chunkCoordinate, false);

    for(const auto& biomeRegionCoord : deactivatedBiomes)
    {
        mWorldData.biomeGrids.erase(biomeRegionCoord);
        mPendingChunksToRequests.erase(biomeRegionCoord); //if the biome is deactivated, we don't need to send pending requests either.
    }

    mWorldData.voxels.erase(chunkCoordinate);

    //we don't need to send pending requests for this chunk
    const BiomeRegionCoord& biomeRegionCoord = ChunkToBiomeRegion::convert(chunkCoordinate);
    auto iterator = mPendingChunksToRequests.find(biomeRegionCoord);

    if(iterator != mPendingChunksToRequests.end())
        std::remove(iterator->second.begin(), iterator->second.end(), chunkCoordinate);
}
