#include "worldprovider.h"
#include "region.h"
#include "../application/applicationmessages.h"

WorldProvider::WorldProvider(fea::MessageBus& b, RegionStorageInterface& mRegionGenerator, ModManager& modManager)
    :
    mBus(b),
    mRegionStorage(mRegionGenerator),
    mModManager(modManager)
{
    mBus.addSubscriber<ChunkRequestedMessage>(*this);
}

WorldProvider::~WorldProvider()
{
    mBus.removeSubscriber<ChunkRequestedMessage>(*this);
}

void WorldProvider::handleMessage(const ChunkRequestedMessage& received)
{
    ChunkCoord chunkCoordinate = received.coordinate;
    RegionCoord regionCoordinate = chunkToRegion(chunkCoordinate);

    if(!mRegionStorage.hasRegion(regionCoordinate))
    {
        Region newRegion = mRegionGenerator.generateRegion(regionCoordinate);
        mBus.send(RegionDeliverMessage{regionCoordinate, newRegion});
    }

    const Region& region = mRegionStorage.getRegion(regionCoordinate);

    Chunk newChunk = mChunkGenerator.generateChunk(chunkCoordinate, region);

    //mBus.send(LogMessage("loading modifications for chunk" + glm::to_string((glm::ivec3)chunkCoordinate), "file", LogLevel::VERB));
    mModManager.loadMods(newChunk);
    uint64_t timestamp = 0; //get proper timestamp later

    mBus.send(ChunkDeliverMessage{chunkCoordinate, newChunk}); //sends the finished chunk to be kept by whatever system

    mBus.send(ChunkLoadedMessage{chunkCoordinate, timestamp}); //the now fully initialised chunk is announced to the rest of the game. should it be here?
}
