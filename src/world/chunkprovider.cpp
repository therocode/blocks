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
    ChunkCoordinate chunkCoordinate;

    std::tie(chunkCoordinate) = received.data;

    RegionCoordinate regionCoordinate = chunkToRegion(chunkCoordinate);

    if(!mRegionStorage.hasRegion(regionCoordinate))
    {
        mBus.sendMessage(RegionNeededMessage(regionCoordinate));
    }

    const Region& region = mRegionStorage.getRegion(regionCoordinate);

    Chunk newChunk(chunkCoordinate);// = mWorldGenerator.generateChunk(chunkCoordinate, region);

    newChunk.setVoxelType(0, 0, 0, 3);
    newChunk.setVoxelType(1, 0, 0, 3);
    newChunk.setVoxelType(2, 0, 0, 3);
    newChunk.setVoxelType(3, 0, 0, 3);
    newChunk.setVoxelType(4, 0, 0, 3);
    newChunk.setVoxelType(5, 0, 0, 3);
    newChunk.setVoxelType(6, 0, 0, 3);
    newChunk.setVoxelType(7, 0, 0, 3);
    newChunk.setVoxelType(8, 0, 0, 3);
    newChunk.setVoxelType(9, 0, 0, 3);
    newChunk.setVoxelType(10, 0, 0, 3);
    newChunk.setVoxelType(11, 0, 0, 3);
    newChunk.setVoxelType(12, 0, 0, 3);
    newChunk.setVoxelType(12, 0, 0, 3);
    newChunk.setVoxelType(13, 0, 0, 3);
    newChunk.setVoxelType(14, 0, 0, 3);
    newChunk.setVoxelType(15, 0, 0, 3);

    //mod manager stuff for the chunk here
    //at this point newChunk is updated with the modifications
    uint64_t timestamp = 0; //get proper timestamp later

    mBus.sendMessage(DeliverChunkMessage(chunkCoordinate, newChunk)); //sends the finished chunk to be kept by whatever system

    mBus.sendMessage(ChunkLoadedMessage(chunkCoordinate, timestamp)); //the now fully initialised chunk is announced to the rest of the game
}
