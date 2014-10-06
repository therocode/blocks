#include "world.hpp"
#include <iostream>
#include <fea/assert.hpp>
#include "worldmessages.hpp"
#include "../application/applicationmessages.hpp"
#include <limits>

int32_t Ranges::MAX = std::numeric_limits<int32_t>::max();
int32_t Ranges::MIN = std::numeric_limits<int32_t>::min();

int32_t added = 0;
int32_t deleted = 0;

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
    ChunkReferenceMap map;

    for(const auto& region : mRegions)
    {
        for(const auto& chunk : region.second.getChunks())
        {
            map.emplace(chunk.second.getLocation(), chunk.second);
        }
    }

    return map;
}

void World::deliverRegion(const RegionCoord& coordinate, const Region& region)
{
    if(mHighlightedRegions.count(coordinate) != 0)
    {
        if(mRegions.count(coordinate) == 0)
        {
            mRegions.emplace(coordinate, std::move(region));
            mBus.send(LogMessage{"Region " + glm::to_string((glm::ivec2)coordinate) + " loaded", "landscape", LogLevel::VERB});

            for(const auto& chunkToRequest : mHighlightedRegions.at(coordinate))
            {
                mBus.send(ChunkGenerationRequestedMessage{0, mId, chunkToRequest, region.getDataFragment(voxelToRegionVoxel(chunkToVoxel(chunkToRequest)), {chunkWidth, chunkWidth})});
            }
        }
    }
}

void World::deliverChunk(const ChunkCoord& coordinate, Chunk& chunk)
{
    RegionCoord regionCoord = chunkToRegion(coordinate);

    if(mHighlightedRegions.count(regionCoord) != 0)
    {
        if(mHighlightedRegions.at(regionCoord).count(coordinate) != 0)
        {
            if(mRegions.count(regionCoord) != 0)
            {
                Region& region = mRegions.at(regionCoord);
                RegionChunkCoord regionChunkCoord = chunkToRegionChunk(coordinate);
                if(!region.hasChunk(regionChunkCoord))
                {
                    region.addChunk(chunkToRegionChunk(coordinate), chunk);

                    //#C1#if(mModManager.hasMods(chunk))  // implement this check as seen in issue #143
                        mModManager.loadMods(chunk);
                    //#C2#else      // implement mExplorationManager as according to issue #144
                    //#C2#{
                    //#C2#    if(!mExplorationManager.isExplored(regionCoord))
                    //#C2#        mBus.send(ChunkInitiallyGeneratedMessage{coordinate, chunk})
                    //#C2#}

                    uint64_t timestamp = 0; //#C3# get proper timestamp, issue #133

                    mBus.send(ChunkCandidateMessage{mId, coordinate, chunk, timestamp});

                    mBus.send(ChunkFinishedMessage{coordinate, chunk}); //the now fully initialised chunk is announced to the rest of the game.
                }
            }
        }
    }
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

    int32_t highlighted = 0;

    for(const auto& region : mHighlightedRegions)
        for(const auto& chunk : region.second)
            highlighted++;

}

const Chunk* World::findChunk(const ChunkCoord& coordinate) const
{
    RegionCoord regionCoord = chunkToRegion(coordinate);

    if(hasRegion(regionCoord))
    {
        const Region& region = mRegions.at(regionCoord);
        RegionChunkCoord regionChunk = chunkToRegionChunk(coordinate);
        if(region.hasChunk(regionChunk))
            return &region.getChunk(regionChunk);
        else
            return nullptr;
    }
    else
        return nullptr;
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
        mBus.send(RegionGenerationRequestedMessage{mId, regionCoord});
    }
    else if(mRegions.count(regionCoord) != 0)
    {
        FEA_ASSERT(mHighlightedRegions.at(regionCoord).count(coordinate) == 0, "Trying to activate chunk " + glm::to_string((glm::ivec3)coordinate) + " but it is already activated");
        mHighlightedRegions.at(regionCoord).insert(coordinate);

        mBus.send(ChunkGenerationRequestedMessage{priority, mId, coordinate, mRegions.at(regionCoord).getDataFragment(voxelToRegionVoxel(chunkToVoxel(coordinate)), {chunkWidth, chunkWidth})});
    }
    else
    {
        mHighlightedRegions.at(regionCoord).insert(coordinate);
    }
}

void World::deactivateChunk(const ChunkCoord& coordinate)
{
    RegionCoord regionCoord = chunkToRegion(coordinate);

    FEA_ASSERT(mHighlightedRegions.count(regionCoord) != 0, "Dehighlighting a region " + glm::to_string((glm::ivec2)regionCoord) + " which has not been highlighted. bad!");

    auto& highlightRegion = mHighlightedRegions.at(regionCoord);

    FEA_ASSERT(highlightRegion.count(coordinate) != 0, "Dehighlighting chunk " + glm::to_string((glm::ivec3)coordinate) + " which has not been highlighted. bad!");

    mHighlightedRegions.at(regionCoord).erase(coordinate);

    bool regionDehighlighted = false;
    //dehighlight, and remove the region if it is empty afterwards
    if(mHighlightedRegions.at(regionCoord).size() == 0)
    {
        mHighlightedRegions.erase(regionCoord);
        regionDehighlighted = true;
    }

    if(mRegions.count(regionCoord) != 0)
    {
        Region& region = mRegions.at(regionCoord);
        if(region.hasChunk(chunkToRegionChunk(coordinate)))
        {
            mModManager.recordTimestamp(coordinate, 0); //timestamp must be fixed
            region.removeChunk(chunkToRegionChunk(coordinate));

            mBus.send(ChunkDeletedMessage{coordinate});
            deleted++;
        }

        if(regionDehighlighted)
        {
            FEA_ASSERT(region.getLoadedChunkAmount() == 0, "Removing a dehighlighted region, but the region contains chunks that haven't been unloaded!\n");
            mBus.send(LogMessage{"saving modifications to disk for region" + glm::to_string((glm::ivec2)regionCoord), "file", LogLevel::VERB});
            mModManager.saveMods(regionCoord);

            mRegions.erase(regionCoord);
        }
    }
}
