#include "worldholder.hpp"
#include "world.hpp"
#include <fea/util.hpp>
#include "../application/applicationmessages.hpp"

WorldHolder::WorldHolder(fea::MessageBus& messageBus, EntitySystem& entitySystem) 
:   mBus(messageBus),
	mWorldInterface(mWorlds, entitySystem),
    mNextId(0)
{
	mBus.addSubscriber<SetVoxelMessage>(*this);
	mBus.addSubscriber<RegionDeliverMessage>(*this);
	mBus.addSubscriber<ChunkDeliverMessage>(*this);
}

WorldHolder::~WorldHolder()
{
	mBus.removeSubscriber<SetVoxelMessage>(*this);
	mBus.removeSubscriber<RegionDeliverMessage>(*this);
	mBus.removeSubscriber<ChunkDeliverMessage>(*this);
}

void WorldHolder::handleMessage(const SetVoxelMessage& received)
{
    bool succeeded = mWorlds.at(received.worldId)->setVoxelType(received.voxel, received.type);

    if(succeeded)
    {
        mBus.send<VoxelSetMessage>(VoxelSetMessage{received.voxel, received.type});
    }
}

void WorldHolder::handleMessage(const RegionDeliverMessage& received)
{
    RegionCoord coordinate = received.coordinate;
    Region region = received.newRegion;

    mWorlds.at(received.worldId)->deliverRegion(coordinate, region);
    mBus.send(LogMessage{"region created" + glm::to_string((glm::ivec2)coordinate), "landscape", LogLevel::VERB});
}

void WorldHolder::handleMessage(const ChunkDeliverMessage& received)
{
    ChunkCoord coordinate = received.coordinate;
    Chunk chunk = received.chunk;

    mWorlds.at(received.worldId)->deliverChunk(coordinate, chunk);

    uint64_t timestamp = 0; //get proper timestamp later
    mBus.send(ChunkLoadedMessage{chunk, timestamp}); //the now fully initialised chunk is announced to the rest of the game. should it be here?
}

WorldInterface& WorldHolder::getWorldInterface()
{
    return mWorldInterface;
}

void WorldHolder::addWorld(const WorldParameters& worldParameters)
{
    mWorldIds.emplace(worldParameters.identifier, mNextId);
    mWorlds.emplace(mNextId, std::unique_ptr<World>(new World(mBus, mNextId, worldParameters.identifier, worldParameters.title, worldParameters.ranges)));

    mNextId++;
}
