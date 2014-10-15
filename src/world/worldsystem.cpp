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
    mBiomeProvider(mBus),
    mNextBiomeIndex(0),
    mNextId(0)
{
    mBus.send(LogMessage{"Setting up world system", worldName, LogLevel::INFO});
    subscribe(mBus, *this);

    //load biomes
    mBus.send(LogMessage{"Loading biomes", worldName, LogLevel::INFO});
    Biome grass("grass", 2,
        {
            {"height", BiomeRequirement(0.0f, 1.0f)},
            {"temperature", BiomeRequirement(0.0f, 1.0f)},
            {"rainfall", BiomeRequirement(0.5f, 1.0f)}
        });

    mBiomes.emplace(mNextBiomeIndex++, grass);
    mBiomeIdentifierToIdMap.emplace(grass.mName, mNextBiomeIndex - 1);

    Biome desert("desert", 3,
        {
            {"height", BiomeRequirement(0.0f, 1.0f)},
            {"temperature", BiomeRequirement(0.0f, 1.0f)},
            {"rainfall", BiomeRequirement(0.0f, 0.5f)}
        });

    mBiomes.emplace(mNextBiomeIndex++, desert);
    mBiomeIdentifierToIdMap.emplace(desert.mName, mNextBiomeIndex - 1);

    mBus.send(BiomesLoadedMessage{mBiomes});

    //load worlds
    WorldLoader mWorldLoader;

    mWorldLoader.loadWorldFile("data/worlds/default.json");

    if(!mWorldLoader.hasError())
    {
        for(const auto& worldParameters : mWorldLoader.getLoadedWorlds())
        {
            createWorld(worldParameters);
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
    mWorlds.at(received.worldId).setVoxelType(received.voxel, received.type);
}

void WorldSystem::handleMessage(const BiomeGeneratedMessage& received)
{
    auto iterator = mWorlds.find(received.worldId);

    if(iterator != mWorlds.end())
        iterator->second.deliverBiome(received.coordinate, received.biomeData);
}

void WorldSystem::handleMessage(const ChunkGeneratedMessage& received)
{
    auto iterator = mWorlds.find(received.worldId);

    if(iterator != mWorlds.end())
        iterator->second.deliverChunk(received.coordinate, received.chunk);
}

void WorldSystem::handleMessage(const HighlightEntityAddRequestedMessage& received)
{
    FEA_ASSERT(mWorlds.count(received.worldId) != 0, "Trying to add a highlight entity to world " + std::to_string(received.worldId) + " but that world does not exist");

    mWorlds.at(received.worldId).addHighlightEntity(received.entityId, received.coordinate);
}

void WorldSystem::handleMessage(const HighlightEntityMoveRequestedMessage& received)
{
    FEA_ASSERT(mWorlds.count(received.worldId) != 0, "Trying to move a highlight entity in world " + std::to_string(received.worldId) + " but that world does not exist");

    mWorlds.at(received.worldId).moveHighlightEntity(received.entityId, received.coordinate);
}

void WorldSystem::handleMessage(const HighlightEntityRemoveRequestedMessage& received)
{
    FEA_ASSERT(mWorlds.count(received.worldId) != 0, "Trying to remove a highlight entity from world " + std::to_string(received.worldId) + " but that world does not exist");

    mWorlds.at(received.worldId).removeHighlightEntity(received.entityId);
}

void WorldSystem::handleMessage(const ChunksRequestedMessage& received)
{
    const auto& iterator = mWorlds.find(received.worldId);
    
    if(iterator != mWorlds.end())
    {
        iterator->second.chunksRequested(received.coordinates);
    }
    else
    {
        ChunksDataDeniedMessage message{received.worldId, received.coordinates};

        mBus.send(message);
    }
}

const ChunkMap& WorldSystem::getWorldVoxels(WorldId id) const
{
    FEA_ASSERT(mWorlds.count(id) != 0, "Trying to get world id " + std::to_string(id) + " but that world does not exist!");
    return mWorlds.at(id).getChunkMap();
}

WorldId WorldSystem::worldIdentifierToId(const std::string& identifier) const
{
    FEA_ASSERT(mIdentifierToIdMap.count(identifier) != 0, "Trying to get the ID of a world called " + identifier + " but that world doesn't exist!");
    return mIdentifierToIdMap.at(identifier);
}

void WorldSystem::createWorld(const WorldParameters& parameters)
{
    mBus.send(LogMessage{"Loading world " + parameters.identifier, worldName, LogLevel::INFO});
    WorldId newId = mNextId++;

    mIdentifierToIdMap.emplace(parameters.identifier, newId);   

    WorldData worldData;

    worldData.range = parameters.ranges;
    worldData.title = parameters.title;

    worldData.biomeSettings.biomes.push_back(mBiomeIdentifierToIdMap.at("grass"));
    worldData.biomeSettings.biomes.push_back(mBiomeIdentifierToIdMap.at("desert"));

    auto createdIterator = mWorlds.emplace(newId, WorldEntry(mBus, newId, parameters.identifier, worldData)).first;
}
