#include "worldprovider.hpp"
#include "region.hpp"
#include "../application/applicationmessages.hpp"

WorldProvider::WorldProvider(fea::MessageBus& b, ModManager& modManager)
    :
    mBus(b),
    mModManager(modManager)
{
    mBus.addSubscriber<ChunkRequestedMessage>(*this);
    mBus.addSubscriber<RegionDeletedMessage>(*this);
    mBus.addSubscriber<FrameMessage>(*this);
}

WorldProvider::~WorldProvider()
{
    mBus.removeSubscriber<ChunkRequestedMessage>(*this);
    mBus.removeSubscriber<RegionDeletedMessage>(*this);
    mBus.removeSubscriber<FrameMessage>(*this);
}

void WorldProvider::handleMessage(const ChunkRequestedMessage& received)
{
    ChunkCoord chunkCoordinate = received.coordinate;
    RegionCoord regionCoordinate = chunkToRegion(chunkCoordinate);

    if(mRegions.count(regionCoordinate) == 0)
    {
        Region newRegion = mRegionGenerator.generateRegion(regionCoordinate);
        mRegions.emplace(regionCoordinate, newRegion);
        mRegionsToDeliver.push_back({regionCoordinate, newRegion});
    }

    const Region& region = mRegions.at(regionCoordinate);

    Chunk newChunk = mChunkGenerator.generateChunk(chunkCoordinate, region);

    //mBus.send(LogMessage("loading modifications for chunk" + glm::to_string((glm::ivec3)chunkCoordinate), "file", LogLevel::VERB));
    mModManager.loadMods(newChunk);

    mChunksToDeliver.push_back({chunkCoordinate, newChunk});
}

void WorldProvider::handleMessage(const RegionDeletedMessage& received)
{
    mRegions.erase(received.coordinate);
}

void WorldProvider::handleMessage(const FrameMessage& received)
{
    if(mRegionsToDeliver.size() > 0)
    {
        for(const auto& region : mRegionsToDeliver)
        {
            mBus.send(RegionDeliverMessage{region.first, std::move(region.second)});
        }
        mRegionsToDeliver.clear();
    }

    if(mChunksToDeliver.size() > 0)
    {
        for(const auto& chunk : mChunksToDeliver)
        {
            mBus.send(ChunkDeliverMessage{chunk.first, std::move(chunk.second)}); //sends the finished chunk to be kept by whatever system

            uint64_t timestamp = 0; //get proper timestamp later
            mBus.send(ChunkLoadedMessage{chunk.first, timestamp}); //the now fully initialised chunk is announced to the rest of the game. should it be here?
        }
        mChunksToDeliver.clear();
    }
}
