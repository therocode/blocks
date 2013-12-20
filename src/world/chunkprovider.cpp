#include "chunkprovider.h"
#include "chunk.h"

ChunkProvider::ChunkProvider(fea::MessageBus& bus, RegionStorageInterface& regionProvider) : mBus(bus), mRegionStorage(regionProvider)
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

    //mod manager stuff for the chunk here
    //at this point newChunk is updated with the modifications
    uint64_t timestamp = 0; //get proper timestamp later

    mBus.sendMessage(ChunkDeliverMessage(chunkCoordinate, newChunk)); //sends the finished chunk to be kept by whatever system

    mBus.sendMessage(ChunkLoadedMessage(chunkCoordinate, timestamp)); //the now fully initialised chunk is announced to the rest of the game
}
