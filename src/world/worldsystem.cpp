#include "worldsystem.hpp"
#include <fea/util.hpp>
#include <fea/assert.hpp>
#include "chunkmap.hpp"
#include "../application/applicationmessages.hpp"
#include "../lognames.hpp"
#include "../world/worldloader.hpp"

WorldSystem::WorldSystem(fea::MessageBus& messageBus) 
:   mBus(messageBus),
    mWorldProvider(mBus),
    mNextId(0)
{
    mBus.send(LogMessage{"Setting up world system", worldName, LogLevel::INFO});
    subscribe(mBus, *this);

    WorldLoader mWorldLoader;

    mWorldLoader.loadWorldFile("data/worlds/default.json");

    if(!mWorldLoader.hasError())
    {
        for(const auto& worldParameters : mWorldLoader.getLoadedWorlds())
        {
            //addWorld(worldParameters); //////this must be fixed
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
    FEA_ASSERT(mWorlds.count(received.worldId) != 0, "Trying to get world id " + std::to_string(received.worldId) + " but that world does not exist!");
    auto& chunks = mWorlds.at(received.worldId).worldData.voxels;
    auto chunk = chunks.find(VoxelToChunk::convert(received.voxel));

    if(chunk != chunks.end())
    {
        chunk->second.setVoxelType(VoxelToChunkVoxel::convert(received.voxel), received.type);
        mBus.send(VoxelSetMessage{received.voxel, received.type});
    }
}

void WorldSystem::handleMessage(const ChunkDeliverMessage& received)
{
    ChunkCoord coordinate = received.coordinate;
    Chunk chunk = received.chunk;

    //check against ranges and active chunks!!!!!
    mWorlds.at(received.worldId).worldData.voxels.emplace(coordinate, chunk);
}

void WorldSystem::handleMessage(const HighlightEntityAddRequestedMessage& received)
{
    FEA_ASSERT(mWorlds.count(received.worldId) != 0, "Trying to add a highlight entity to world " + std::to_string(received.worldId) + " but that world does not exist");

    //hook up to something
}

void WorldSystem::handleMessage(const HighlightEntityMoveRequestedMessage& received)
{
    FEA_ASSERT(mWorlds.count(received.worldId) != 0, "Trying to move a highlight entity in world " + std::to_string(received.worldId) + " but that world does not exist");

    //hook up to something
}

void WorldSystem::handleMessage(const HighlightEntityRemoveRequestedMessage& received)
{
    FEA_ASSERT(mWorlds.count(received.worldId) != 0, "Trying to remove a highlight entity from world " + std::to_string(received.worldId) + " but that world does not exist");

    //hook up to something
}

const ChunkMap& WorldSystem::getWorldVoxels(WorldId id) const
{
    FEA_ASSERT(mWorlds.count(id) != 0, "Trying to get world id " + std::to_string(id) + " but that world does not exist!");
    return mWorlds.at(id).worldData.voxels;
}
