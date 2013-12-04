#include "world.h"
#include "worldmessages.h"
#include "localchunkprovider.h"

World::World(fea::MessageBus& messageBus) : bus(messageBus),
                                                    landscape(messageBus)
{

}

void World::initialise()
{
    landscape.setChunkProvider(std::unique_ptr<ChunkProvider>(new LocalChunkProvider()));
}

void World::highlightChunk(size_t id, const ChunkCoordinate& chunk)
{
    landscape.highlightChunk(id, chunk);
}

const Region& World::getLandscape() const
{
    return landscape;
}

Region& World::getLandscape()
{
    return landscape;
}
