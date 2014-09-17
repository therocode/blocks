#include "world.hpp"
#include <iostream>
#include <fea/assert.hpp>
#include "worldmessages.hpp"
#include "../application/applicationmessages.hpp"
#include <limits>

int32_t Ranges::MAX = std::numeric_limits<int32_t>::max();
int32_t Ranges::MIN = std::numeric_limits<int32_t>::min();

Ranges::Ranges(const IntRange& x, const IntRange& y, const IntRange& z) :
    xRange(x),
    yRange(y),
    zRange(z)
{
    FEA_ASSERT(x.first <= x.second, "Invalid X range in range!");
    FEA_ASSERT(y.first <= y.second, "Invalid Y range in range!");
    FEA_ASSERT(z.first <= z.second, "Invalid Z range in range!");
}

bool Ranges::isWithin(const glm::ivec3& coordinate) const
{
    return (coordinate.x >= xRange.first && coordinate.x <= xRange.second &&
            coordinate.y >= yRange.first && coordinate.y <= yRange.second &&
            coordinate.z >= zRange.first && coordinate.z <= zRange.second);   
}

World::World(fea::MessageBus& b, WorldId id, const std::string& textIdentifier, const std::string& title, const Ranges& ranges) :
    mBus(b),
    mId(id),
    mTextIdentifier(textIdentifier),
    mTitle(title),
    mHighlightManager(8),
    mWorldRange(ranges),
    mModManager(b, textIdentifier)
{
}

World::~World()
{
    mModManager.saveMods();
    mBus.send(LogMessage{std::string("saving modifications to disk for all regions in world " + mTextIdentifier), "file", LogLevel::VERB});
}

ChunkReferenceMap World::getChunkMap() const
{
    return ChunkReferenceMap(); //this needs to be fixed for network chunks to be displayed
}

void World::deliverRegion(const RegionCoord& coordinate, const Region& region)
{
    mRegions.emplace(coordinate, std::move(region));
    mBus.send(LogMessage{"Region " + glm::to_string((glm::ivec2)coordinate) + " loaded", "file", LogLevel::VERB});
}

void World::deliverChunk(const ChunkCoord& coordinate, Chunk& chunk)
{
    RegionCoord region = chunkToRegion(coordinate);

    FEA_ASSERT(mHighlightedRegions.count(region) != 0, "Trying to deliver chunk " + glm::to_string((glm::ivec3)coordinate) + " which  belongs to region " + glm::to_string((glm::ivec2)region) + " but that region is not highlighted!");
    FEA_ASSERT(mHighlightedRegions.at(region).count(coordinate) != 0, "Got chunk " + glm::to_string((glm::ivec3)coordinate) + " delivered but that chunk is not highlighted!");
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
    {
        if(mWorldRange.isWithin((glm::ivec3)chunk))
            activateChunk(chunk, (int32_t)glm::distance((glm::vec3)coordinate, (glm::vec3)chunk));
    }
}

void World::removeHighlightEntity(fea::EntityId id)
{
    ChunkDehighlightList dehighlighted = mHighlightManager.removeHighlightEntity(id);

    for(const auto& chunk : dehighlighted)
    {
        if(mWorldRange.isWithin((glm::ivec3)chunk))
            deactivateChunk(chunk);
    }
}

void World::moveHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate)
{
    std::pair<ChunkHighlightList, ChunkDehighlightList> highlightInfo = mHighlightManager.moveHighlightEntity(id, coordinate);

    for(const auto& chunk : highlightInfo.first)
    {
        if(mWorldRange.isWithin((glm::ivec3)chunk))
            activateChunk(chunk, (int32_t)glm::distance((glm::vec3)coordinate, (glm::vec3)chunk));
    }

    for(const auto& chunk : highlightInfo.second)
    {
        if(mWorldRange.isWithin((glm::ivec3)chunk))
            deactivateChunk(chunk);
    }
}

bool World::hasRegion(const RegionCoord& coordinate) const
{
    return mRegions.count(coordinate) != 0;
}

void World::activateChunk(const ChunkCoord& coordinate, int32_t priority)
{
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

    mBus.send(ChunkRequestedMessage{priority, mId, coordinate});
}

void World::deactivateChunk(const ChunkCoord& coordinate)
{
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
        mBus.send(HaltChunkAndRegionGenerationMessage{mId, coordinate, regionRemoved ? &regionCoord : nullptr});
        return; //return, because there is nothing to remove
    }

    auto& region = mRegions.at(regionCoord);

    //if a chunk is deactivated and doesn't exist, it has to be under generation in the other thread. notify the thread to stop generation
    if(region.hasChunk(chunkToRegionChunk(coordinate)) == 0)
    {
        mBus.send(HaltChunkAndRegionGenerationMessage{mId, coordinate, nullptr});
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
        mBus.send(RegionDeletedMessage{mId, regionCoord});

        mBus.send(LogMessage{"saving modifications to disk for region" + glm::to_string((glm::ivec2)regionCoord), "file", LogLevel::VERB});
        mModManager.saveMods(regionCoord);
    }
}
