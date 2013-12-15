#include "chunkprovider.h"

ChunkProvider::ChunkProvider(fea::MessageBus& bus, RegionProviderInterface& regionProvider) : mBus(bus), mRegionProvider(regionProvider)
{
}

void ChunkProvider::handleMessage(const ChunkRequestedMessage& received)
{
    ChunkCoordinate chunkCoordinate;

    std::tie(chunkCoordinate) = received.data;

    RegionCoordinate regionCoordinate = chunkToRegion(chunkCoordinate);

    if(!mRegionProvider.hasRegion(regionCoordinate))
    {
        mBus.sendMessage(RegionNeededMessage(regionCoordinate));
    }

    const Region& region = mRegionProvider.getRegion(regionCoordinate);

    Chunk newChunk;// = mWorldGenerator.generateChunk(chunkCoordinate, region);

    //mod manager stuff for the chunk here
    //at this point newChunk is updated with the modifications
    uint64_t timestamp = 0; //get proper timestamp later

    mBus.sendMessage(DeliverChunkMessage(chunkCoordinate, newChunk)); //sends the finished chunk to be kept by whatever system

    mBus.sendMessage(ChunkLoadedMessage(chunkCoordinate, timestamp)); //the now fully initialised chunk is announced to the rest of the game
}
