#include "worldsystem.hpp"
#include <fea/util.hpp>
#include <fea/assert.hpp>
#include "chunkmap.hpp"
#include "../application/applicationmessages.hpp"
#include "../lognames.hpp"
#include "../utilities/directorycreator.hpp"

WorldSystem::WorldSystem(fea::MessageBus& messageBus) 
:   mBus(messageBus),
    mChunkProvider(mBus),
    mBiomeProvider(mBus),
    mNextId(0),
	mWorldPath("worlds")
{
    mBus.send(LogMessage{"Setting up world system", gWorldName, LogLevel::INFO});
    subscribe(mBus, *this);

    //load biomes
    mBus.send(LogMessage{"Loading biomes", gWorldName, LogLevel::INFO});
    Biome grass(mBiomeIds.getId("grass"), 2,
        {
            {mFieldIds.getId("height"), BiomeRequirement(0.0f, 1.0f)},
            {mFieldIds.getId("temperature"), BiomeRequirement(0.0f, 1.0f)},
            {mFieldIds.getId("rainfall"), BiomeRequirement(0.5f, 1.0f)}
        });

    mBiomes.emplace(grass.mId, grass);

    Biome desert(mBiomeIds.getId("desert"), 3,
        {
            {mFieldIds.getId("height"), BiomeRequirement(0.0f, 1.0f)},
            {mFieldIds.getId("temperature"), BiomeRequirement(0.0f, 1.0f)},
            {mFieldIds.getId("rainfall"), BiomeRequirement(0.0f, 0.5f)}
        });

    mBiomes.emplace(desert.mId, desert);

    mBus.send(BiomesLoadedMessage{mBiomes});


    //make sure world data directory exists. In the future, the name of it should be a setting
    if(!DirectoryCreator::directoryExists(mWorldPath))
    {
        mBus.send(LogMessage{"Creating directory '" + mWorldPath + "' to store worlds in", gWorldName, LogLevel::INFO});
        bool success = DirectoryCreator::createDirectory(mWorldPath);

        if(!success)
            mBus.send(LogMessage{"Failed creating world data directory '" + mWorldPath + "'!", gWorldName, LogLevel::ERR});
    }
}

WorldSystem::~WorldSystem()
{
    mBus.send(LogMessage{"Shutting down world system", gWorldName, LogLevel::INFO});
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
        iterator->second.deliverBiome(received.coordinate, received.biomeData, received.fields);
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

    mWorlds.at(received.worldId).addHighlightEntity(received.entityId, received.coordinate, received.radius);
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

void WorldSystem::handleMessage(const ResourceDeliverMessage<std::vector<WorldParameters>>& received)
{
	for(const auto& worldParameters : *received.resource)
	{
		createWorld(worldParameters);
	}
}

const ChunkMap& WorldSystem::getWorldVoxels(WorldId id) const
{
    FEA_ASSERT(mWorlds.count(id) != 0, "Trying to get world id " + std::to_string(id) + " but that world does not exist!");
    return mWorlds.at(id).getChunkMap();
}

bool WorldSystem::hasWorld(const std::string& identifier) const
{
    return mIdentifierToIdMap.count(identifier) != 0;
}

WorldId WorldSystem::worldIdentifierToId(const std::string& identifier) const
{
    FEA_ASSERT(mIdentifierToIdMap.count(identifier) != 0, "Trying to get the ID of a world called " + identifier + " but that world doesn't exist!");
    return mIdentifierToIdMap.at(identifier);
}

const std::string& WorldSystem::worldIdToIdentifier(WorldId id) const
{
    FEA_ASSERT(mWorlds.count(id) != 0, "Trying to get the identifier of a world id " + std::to_string(id) + " but that world doesn't exist!");
    return mWorlds.at(id).getIdentifier();
}

void WorldSystem::createWorld(const WorldParameters& parameters)
{
    mBus.send(LogMessage{"Loading world " + parameters.identifier, gWorldName, LogLevel::INFO});
    WorldId newId = mNextId++;

    mIdentifierToIdMap.emplace(parameters.identifier, newId);   

    WorldData worldData;

    worldData.range = parameters.ranges;
    worldData.title = parameters.title;

    worldData.biomeSettings.biomes.push_back(mBiomeIds.getId("grass"));
    worldData.biomeSettings.biomes.push_back(mBiomeIds.getId("desert"));

    //make sure world directory exists.
    std::string path = mWorldPath + "/" + parameters.identifier;
    if(!DirectoryCreator::directoryExists(path))
    {
        mBus.send(LogMessage{"Creating directory '" + path + "' for world " + parameters.identifier, gWorldName, LogLevel::INFO});
        bool success = DirectoryCreator::createDirectory(path);

        if(!success)
            mBus.send(LogMessage{"Failed creating world directory '" + path + "'!", gWorldName, LogLevel::ERR});
    }

    worldData.biomeSettings.fields =
    {
        Field{mFieldIds.getId("height"), NoiseType::SIMPLEX, false},
        Field{mFieldIds.getId("rainfall"), NoiseType::SIMPLEX, false},
        Field{mFieldIds.getId("temperature"), NoiseType::SIMPLEX, false},
        Field{mFieldIds.getId("selector"), NoiseType::VORONOI, true}
    };


    auto createdIterator = mWorlds.emplace(newId, WorldEntry(mBus, mBiomes, newId, parameters.identifier, worldData, path)).first;
}
