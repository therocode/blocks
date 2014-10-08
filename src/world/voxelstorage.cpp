#include "voxelstorage.hpp"
#include <fea/assert.hpp>
#include <limits>

World::World(const std::string& title, const Ranges& ranges) :
    mTitle(title),
    mWorldRange(ranges)
{
}

void World::addChunk(const ChunkCoord& coordinate, const Chunk& chunk)
{
    FEA_ASSERT(mChunks.count(coordinate) == 0, "Trying to add a chunk which already exists!");
    FEA_ASSERT(mWorldRange.isWithin(coordinate), "Trying to add a chunk without the range of the world!");

    mChunks.emplace(coordinate, chunk);
}

const Chunk* World::findChunk(const ChunkCoord& coordinate) const
{
    auto chunk = mChunks.find(coordinate);
    return chunk != mChunks.end() ? &chunk->second : nullptr;
}

void World::removeChunk(const ChunkCoord& coordinate)
{
    FEA_ASSERT(mChunks.count(coordinate) != 0, "Trying to remove a non-existing chunk!");

    mChunks.erase(coordinate);
}
