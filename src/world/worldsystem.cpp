#include "worldsystem.hpp"
#include "world.hpp"
#include <fea/util.hpp>
#include "../application/applicationmessages.hpp"
#include "../lognames.hpp"
#include "../world/worldloader.hpp"

WorldSystem::WorldSystem(fea::MessageBus& messageBus, EntitySystem& entitySystem) 
:   mBus(messageBus),
	mWorldInterface(mWorlds, entitySystem),
    mNextId(0),
    mWorldProvider(mBus)
{
    mBus.send(LogMessage{"Setting up world system", worldName, LogLevel::INFO});
    subscribe(mBus, *this);

    WorldLoader mWorldLoader;

    mWorldLoader.loadWorldFile("data/worlds/default.json");

    if(!mWorldLoader.hasError())
    {
        for(const auto& worldParameters : mWorldLoader.getLoadedWorlds())
        {
            addWorld(worldParameters);
        }
    }
    else
    {
        mBus.send(LogMessage{"World loading error: " + mWorldLoader.getErrorString(), worldName, LogLevel::ERR});
    }
    
}

WorldSystem::~WorldSystem()
{
    mBus.send(LogMessage{"Shutting down world system", worldName, LogLevel::INFO});
}

void WorldSystem::handleMessage(const SetVoxelMessage& received)
{
    bool succeeded = mWorlds.at(received.worldId)->setVoxelType(received.voxel, received.type);

    if(succeeded)
    {
        mBus.send(VoxelSetMessage{received.voxel, received.type});
    }
}

void WorldSystem::handleMessage(const RegionDeliverMessage& received)
{
    RegionCoord coordinate = received.coordinate;
    Region region = received.newRegion;

    mWorlds.at(received.worldId)->deliverRegion(coordinate, region);
}

void WorldSystem::handleMessage(const ChunkDeliverMessage& received)
{
    ChunkCoord coordinate = received.coordinate;
    Chunk chunk = received.chunk;

    mWorlds.at(received.worldId)->deliverChunk(coordinate, chunk);
}

WorldInterface& WorldSystem::getWorldInterface()
{
    return mWorldInterface;
}

void WorldSystem::addWorld(const WorldParameters& worldParameters)
{
    mBus.send(LogMessage{"Loading world " + worldParameters.identifier, worldName, LogLevel::INFO});

    mWorldIds.emplace(worldParameters.identifier, mNextId);
    mWorlds.emplace(mNextId, std::unique_ptr<World>(new World(mBus, mNextId, worldParameters.identifier, worldParameters.title, worldParameters.ranges)));

    mNextId++;
}
