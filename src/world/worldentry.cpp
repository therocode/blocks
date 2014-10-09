#include "worldentry.hpp"

WorldEntry::WorldEntry(const std::string& identifier) :
    mHighlightManager(8), //this should be read from somewhere else, probably server settings
    mModManager(identifier)
{
}

void WorldEntry::addHighlightEntity(uint32_t id, const ChunkCoord& location)
{
    const auto& highlighted = mHighlightManager.addHighlightEntity(id, location);

    for(const auto& chunk : highlighted)
        activateChunk(chunk);
}

void WorldEntry::moveHighlightEntity(uint32_t id, const ChunkCoord& location)
{
    const auto& highlightedDehighlighted = mHighlightManager.moveHighlightEntity(id, location);

    for(const auto& chunk : highlightedDehighlighted.first)
        activateChunk(chunk);

    for(const auto& chunk : highlightedDehighlighted.second)
        deactivateChunk(chunk);
}


void WorldEntry::removeHighlightEntity(uint32_t id)
{
    const auto& dehighlighted = mHighlightManager.removeHighlightEntity(id);

    for(const auto& chunk : dehighlighted)
        deactivateChunk(chunk);
}

const ChunkMap& WorldEntry::getChunkMap() const
{
    return mWorldData.voxels;
}

ChunkMap& WorldEntry::getChunkMap()
{
    return mWorldData.voxels;
}

void WorldEntry::activateChunk(const ChunkCoord& chunkCoordinate)
{
    //send biome requests and chunk requests
}

void WorldEntry::deactivateChunk(const ChunkCoord& chunkCoordinate)
{
}
