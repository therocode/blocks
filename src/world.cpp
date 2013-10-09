#include "world.h"
        
World::World(fea::MessageBus& messageBus) : bus(messageBus)
{
}

void World::initialise()
{
    ChunkCoordinate coords;
    coords.x = 0;
    coords.y = 0;
    coords.z = 0;

    Chunk newChunk;
    chunks.push_back(newChunk);

    bus.sendMessage<ChunkCreatedMessage>(ChunkCreatedMessage(&coords,&newChunk));
}

void World::update()
{
}

void World::hehehe()
{
}
