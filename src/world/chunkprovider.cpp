#include "chunkprovider.h"
#include "chunk.h"
#include "application/applicationmessages.h"

ChunkProvider::ChunkProvider(fea::MessageBus& bus, RegionStorageInterface& regionProvider, ModManager& modManager) : mBus(bus), mRegionStorage(regionProvider), mModManager(modManager)
{
    mBus.addSubscriber<ChunkRequestedMessage>(*this);
}

ChunkProvider::~ChunkProvider()
{
    mBus.removeSubscriber<ChunkRequestedMessage>(*this);
}

void ChunkProvider::handleMessage(const ChunkRequestedMessage& received)
{
    ChunkCoord chunkCoordinate = received.coordinate;

    RegionCoord regionCoordinate = chunkToRegion(chunkCoordinate);

    if(!mRegionStorage.hasRegion(regionCoordinate))
    {
        mBus.send(RegionNeededMessage{regionCoordinate});
    }

    const Region& region = mRegionStorage.getRegion(regionCoordinate);

    Chunk newChunk = mWorldGenerator.generateChunk(chunkCoordinate, region);

    //mBus.send(LogMessage("loading modifications for chunk" + glm::to_string((glm::ivec3)chunkCoordinate), "file", LogLevel::VERB));
    mModManager.loadMods(newChunk);
    uint64_t timestamp = 0; //get proper timestamp later

    mBus.send(ChunkDeliverMessage{chunkCoordinate, newChunk}); //sends the finished chunk to be kept by whatever system

    mBus.send(ChunkLoadedMessage{chunkCoordinate, timestamp}); //the now fully initialised chunk is announced to the rest of the game
}
