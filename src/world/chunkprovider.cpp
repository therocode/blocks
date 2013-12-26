#include "chunkprovider.h"
#include "chunk.h"
#include "application/applicationmessages.h"

ChunkProvider::ChunkProvider(fea::MessageBus& bus, RegionStorageInterface& regionProvider, ModManager& modManager) : mBus(bus), mRegionStorage(regionProvider), mModManager(modManager)
{
    mBus.addMessageSubscriber<ChunkRequestedMessage>(*this);
}

ChunkProvider::~ChunkProvider()
{
    mBus.removeMessageSubscriber<ChunkRequestedMessage>(*this);
}

void ChunkProvider::handleMessage(const ChunkRequestedMessage& received)
{
    ChunkCoord chunkCoordinate;

    std::tie(chunkCoordinate) = received.data;

    RegionCoord regionCoordinate = chunkToRegion(chunkCoordinate);

    if(!mRegionStorage.hasRegion(regionCoordinate))
    {
        mBus.sendMessage(RegionNeededMessage(regionCoordinate));
    }

    const Region& region = mRegionStorage.getRegion(regionCoordinate);

    Chunk newChunk = mWorldGenerator.generateChunk(chunkCoordinate, region);

    //mBus.sendMessage(LogMessage("loading modifications for chunk" + glm::to_string((glm::ivec3)chunkCoordinate), "file", LogLevel::VERB));
    mModManager.loadMods(newChunk);
    uint64_t timestamp = 0; //get proper timestamp later

    mBus.sendMessage(ChunkDeliverMessage(chunkCoordinate, newChunk)); //sends the finished chunk to be kept by whatever system

    mBus.sendMessage(ChunkLoadedMessage(chunkCoordinate, timestamp)); //the now fully initialised chunk is announced to the rest of the game
}
