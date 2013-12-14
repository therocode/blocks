#include "world.h"
#include "worldmessages.h"
#include "localchunkprovider.h"

World::World(fea::MessageBus& messageBus) : bus(messageBus)
{

}

void World::initialise()
{
}

void World::highlightChunk(size_t id, const ChunkCoordinate& chunk)
{
    RegionCoordinate regionCoordinate(chunk);

    auto region = mRegions.find(regionCoordinate);

    if(region == mRegions.end())
    {
        mRegions.emplace(regionCoordinate, loadRegion(regionCoordinate));
        region = mRegions.find(regionCoordinate);
    }
    region->second.highlightChunk(id, ChunkRegionCoordinate(chunk.x, chunk.y, chunk.z));
}

ChunkReferenceMap World::getChunkMap() const
{
    return ChunkReferenceMap();
}
