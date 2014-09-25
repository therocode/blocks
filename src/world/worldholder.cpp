#include "worldholder.hpp"
#include "world.hpp"
#include <fea/util.hpp>
#include "../application/applicationmessages.hpp"

WorldHolder::WorldHolder(fea::MessageBus& messageBus, EntitySystem& entitySystem) 
:   mBus(messageBus),
	mWorldInterface(mWorlds, entitySystem),
    mNextId(0)
{
    subscribe(mBus, *this);
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
}

void WorldHolder::handleMessage(const ChunkDeliverMessage& received)
{
    ChunkCoord coordinate = received.coordinate;
    Chunk chunk = received.chunk;

    mWorlds.at(received.worldId)->deliverChunk(coordinate, chunk);
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
