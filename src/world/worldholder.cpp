#include "worldholder.hpp"
#include <fea/util.hpp>
#include "../application/applicationmessages.hpp"

WorldHolder::WorldHolder(fea::MessageBus& messageBus, EntitySystem& entitySystem) 
:   mBus(messageBus),
	mWorldInterface(mWorlds, entitySystem)
{
	mBus.addSubscriber<SetVoxelMessage>(*this);
	mBus.addSubscriber<RegionDeliverMessage>(*this);
	mBus.addSubscriber<ChunkDeliverMessage>(*this);

    mWorlds.emplace("default", World(mBus, "default"));
}

WorldHolder::~WorldHolder()
{
	mBus.removeSubscriber<SetVoxelMessage>(*this);
	mBus.removeSubscriber<RegionDeliverMessage>(*this);
	mBus.removeSubscriber<ChunkDeliverMessage>(*this);
}

void WorldHolder::handleMessage(const SetVoxelMessage& received)
{
    VoxelCoord coordinate = received.voxel;
    VoxelType type = received.type;

    bool succeeded = mWorlds.at("default").setVoxelType(coordinate, type);

    if(succeeded)
    {
        mBus.send<VoxelSetMessage>(VoxelSetMessage{coordinate, type});
    }
}

void WorldHolder::handleMessage(const RegionDeliverMessage& received)
{
    RegionCoord coordinate = received.coordinate;
    Region region = received.newRegion;

    mWorlds.at("default").addRegion(coordinate, region);
    mBus.send(LogMessage{"region created" + glm::to_string((glm::ivec2)coordinate), "landscape", LogLevel::VERB});
}

void WorldHolder::handleMessage(const ChunkDeliverMessage& received)
{
    ChunkCoord coordinate = received.coordinate;
    Chunk chunk = received.chunk;

    mWorlds.at("default").addChunk(coordinate, chunk);

    uint64_t timestamp = 0; //get proper timestamp later
    mBus.send(ChunkLoadedMessage{chunk, timestamp}); //the now fully initialised chunk is announced to the rest of the game. should it be here?
}

WorldInterface& WorldHolder::getWorldInterface()
{
    return mWorldInterface;
}
