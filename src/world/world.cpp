#include "world.hpp"
#include <iostream>
#include <fea/assert.hpp>
#include "worldmessages.hpp"
#include "../application/applicationmessages.hpp"

World::World(fea::MessageBus& b, const std::string& identifier) :
    mBus(b),
    mIdentifier(identifier),
    mHighlightManager(8),
    mModManager(b, identifier)
{
}

World::~World()
{
    mModManager.saveMods();
    mBus.send(LogMessage{std::string("saving modifications to disk for all regions in world " + mIdentifier), "file", LogLevel::VERB});
}

ChunkReferenceMap World::getChunkMap() const
{
    return ChunkReferenceMap(); //this needs to be fixed for network chunks to be displayed
}

void World::deliverRegion(const RegionCoord& coordinate, const Region& region)
{
    mRegions.emplace(coordinate, std::move(region));
}

void World::deliverChunk(const ChunkCoord& coordinate, Chunk& chunk)
{
    RegionCoord region = chunkToRegion(coordinate);

    FEA_ASSERT(mRegions.count(region) != 0, "Trying to deliver chunk " + glm::to_string((glm::ivec3)coordinate) + " which belongs to region " + glm::to_string((glm::ivec2)region) + " but that region doesn't exist!");

    mModManager.loadMods(chunk);

    mRegions.at(region).addChunk(chunkToRegionChunk(coordinate), chunk);
}

VoxelType World::getVoxelType(const VoxelCoord& voxelCoordinate) const
{
    RegionCoord regionCoord = voxelToRegion(voxelCoordinate);

    if(hasRegion(regionCoord))
    {
        const Region& region = mRegions.at(regionCoord);
        RegionChunkCoord chunk = voxelToRegionChunk(voxelCoordinate);
        if(region.hasChunk(chunk))
            return region.getChunk(chunk).getVoxelType(voxelToChunkVoxel(voxelCoordinate));
        else
            return -1;
    }
    else
        return -1;
}

bool World::setVoxelType(const VoxelCoord& voxelCoordinate, VoxelType type)
{
    RegionCoord regionCoord = voxelToRegion(voxelCoordinate);

    if(hasRegion(regionCoord))
    {
        Region& region = mRegions.at(regionCoord);
        RegionChunkCoord chunk = voxelToRegionChunk(voxelCoordinate);
        if(region.hasChunk(chunk))
        {
            region.getChunk(chunk).setVoxelType(voxelToChunkVoxel(voxelCoordinate), type);
            mModManager.setMod(voxelToChunk(voxelCoordinate), voxelToChunkVoxel(voxelCoordinate), type);
            //mBus.send(LogMessage{"Set voxel in chunk " + glm::to_string((glm::ivec3)voxelToChunk(voxelCoordinate)), "landscape", LogLevel::VERB});
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

void World::addHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate)
{
    ChunkHighlightList highlighted = mHighlightManager.addHighlightEntity(id, coordinate);

    for(const auto& chunk : highlighted)
        activateChunk(chunk);
}

void World::removeHighlightEntity(fea::EntityId id)
{
    ChunkDehighlightList dehighlighted = mHighlightManager.removeHighlightEntity(id);

    for(const auto& chunk : dehighlighted)
        deactivateChunk(chunk);
}

void World::moveHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate)
{
    std::pair<ChunkHighlightList, ChunkDehighlightList> highlightInfo = mHighlightManager.moveHighlightEntity(id, coordinate);

    for(const auto& chunk : highlightInfo.first)
        activateChunk(chunk);

    for(const auto& chunk : highlightInfo.second)
        deactivateChunk(chunk);
}

bool World::hasRegion(const RegionCoord& coordinate) const
{
    return mRegions.count(coordinate) != 0;
}

void World::activateChunk(const ChunkCoord& coordinate)
{
    //mBus.send(LogMessage{"Highlighted chunk " + glm::to_string((glm::ivec3)coordinate), "landscape", LogLevel::VERB});

    RegionCoord regionCoord = chunkToRegion(coordinate);

    if(mHighlightedRegions.count(regionCoord) == 0)
    {
        mHighlightedRegions.emplace(regionCoord, std::unordered_set<ChunkCoord>{coordinate});
    }
    else
    {
        FEA_ASSERT(mHighlightedRegions.at(regionCoord).count(coordinate) == 0, "Trying to activate chunk " + glm::to_string((glm::ivec3)coordinate) + " but it is already activated");
        mHighlightedRegions.at(regionCoord).insert(coordinate);
    }

    mBus.send(ChunkRequestedMessage{mIdentifier, coordinate});
}

void World::deactivateChunk(const ChunkCoord& coordinate)
{
    //mBus.send(LogMessage{"Dehighlighted chunk " + glm::to_string((glm::ivec3)coordinate), "landscape", LogLevel::VERB});
    RegionCoord regionCoord = chunkToRegion(coordinate);

    FEA_ASSERT(mHighlightedRegions.count(regionCoord) != 0, "Dehighlighting a region " + glm::to_string((glm::ivec2)regionCoord) + " which has not been highlighted. bad!");

    auto& highlightRegion = mHighlightedRegions.at(regionCoord);

    FEA_ASSERT(highlightRegion.count(coordinate) != 0, "Dehighlighting chunk " + glm::to_string((glm::ivec3)coordinate) + " which has not been highlighted. bad!");

    //dehighlight, and remove the region if it is empty afterwards
    mHighlightedRegions.at(regionCoord).erase(coordinate);

    bool regionRemoved = false;

    if(mHighlightedRegions.at(regionCoord).size() == 0)
    {
        mHighlightedRegions.erase(regionCoord);
        regionRemoved = true;
    }

    //if a chunk is dehighlighted and there is no region for that chunk, it must mean that the region is about to be generated in the other thread but hasn't been finished yet. The thread must be notified that the region and chunk is not needed anymore. But only if it is not needed by another chunk
    if(mRegions.count(regionCoord) == 0)
    {
        mBus.send(HaltChunkAndRegionGenerationMessage{coordinate, regionRemoved ? &regionCoord : nullptr});
        //mBus.send(LogMessage{"Haltting generation of " + glm::to_string((glm::ivec3)coordinate), "landscape", LogLevel::VERB});
        return; //return, because there is nothing to remove
    }

    auto& region = mRegions.at(regionCoord);

    //if a chunk is deactivated and doesn't exist, it has to be under generation in the other thread. notify the thread to stop generation
    if(region.hasChunk(chunkToRegionChunk(coordinate)) == 0)
    {
        mBus.send(HaltChunkAndRegionGenerationMessage{coordinate, nullptr});
        //mBus.send(LogMessage{"Halting generation of " + glm::to_string((glm::ivec3)coordinate), "landscape", LogLevel::VERB});
        return; //return, because there is nothing to remove
    }

    mModManager.recordTimestamp(coordinate, 0); //timestamp must be fixed
    removeChunk(coordinate);

    mBus.send(ChunkDeletedMessage{coordinate});
}

void World::removeChunk(const ChunkCoord& coordinate)
{
    RegionCoord regionCoord = chunkToRegion(coordinate);

    FEA_ASSERT(mRegions.count(regionCoord) != 0, "Removing chunk " + glm::to_string((glm::ivec3)coordinate) + " from the world in region " + glm::to_string((glm::ivec2)regionCoord) + " but that region has not been added");

    Region& region = mRegions.at(regionCoord);

    FEA_ASSERT(region.hasChunk(chunkToRegionChunk(coordinate)), "Removing chunk " + glm::to_string((glm::ivec3)coordinate) + " from the world in region " + glm::to_string((glm::ivec2)regionCoord) + " but that chunk has not been added");

    region.removeChunk(chunkToRegionChunk(coordinate));

    if(region.getLoadedChunkAmount() == 0)
    {
        mRegions.erase(regionCoord);
        mBus.send(RegionDeletedMessage{regionCoord});

        mBus.send(LogMessage{"saving modifications to disk for region" + glm::to_string((glm::ivec2)regionCoord), "file", LogLevel::VERB});
        mModManager.saveMods(regionCoord);
    }
}
