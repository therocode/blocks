#include "worldsystem.hpp"
#include "voxelstorage.hpp"
#include <fea/util.hpp>
#include "../application/applicationmessages.hpp"
#include "../lognames.hpp"
#include "../world/worldloader.hpp"

WorldSystem::WorldSystem(fea::MessageBus& messageBus) 
:   mBus(messageBus),
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

void WorldSystem::handleMessage(const HighlightEntityAddRequestedMessage& received)
{
    FEA_ASSERT(mWorlds.count(received.worldId) != 0, "Trying to add a highlight entity to world " + std::to_string(received.worldId) + " but that world does not exist");
    mWorlds.at(received.worldId)->addHighlightEntity(received.entityId, received.coordinate); 
}

void WorldSystem::handleMessage(const HighlightEntityMoveRequestedMessage& received)
{
    FEA_ASSERT(mWorlds.count(received.worldId) != 0, "Trying to move a highlight entity in world " + std::to_string(received.worldId) + " but that world does not exist");
    mWorlds.at(received.worldId)->moveHighlightEntity(received.entityId, received.coordinate); 
}

void WorldSystem::handleMessage(const HighlightEntityRemoveRequestedMessage& received)
{
    FEA_ASSERT(mWorlds.count(received.worldId) != 0, "Trying to remove a highlight entity from world " + std::to_string(received.worldId) + " but that world does not exist");
    mWorlds.at(received.worldId)->removeHighlightEntity(received.entityId); 
}

void WorldSystem::addWorld(const WorldParameters& worldParameters)
{
    mBus.send(LogMessage{"Loading world " + worldParameters.identifier, worldName, LogLevel::INFO});

    mWorldIds.emplace(worldParameters.identifier, mNextId);
    mWorlds.emplace(mNextId, std::unique_ptr<World>(new World(mBus, mNextId, worldParameters.identifier, worldParameters.title, worldParameters.ranges)));

    mNextId++;
}

const World& WorldSystem::getWorld(WorldId id) const
{
    FEA_ASSERT(mWorlds.count(id) != 0, "Trying to get world id " + std::to_string(id) + " but that world does not exist!");
    return *mWorlds.at(id);
}
