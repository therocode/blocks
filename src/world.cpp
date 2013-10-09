#include "world.h"
        
World::World(fea::MessageBus& messageBus) : bus(messageBus)
{
}

void World::initialise()
{
    for(int32_t x = -4; x < 5; x++)
    {
        for(int32_t y = -4; y < 5; y++)
        {
            for(int32_t z = -4; z < 5; z++)
            {
                ChunkCoordinate coords(x, y, z);

                Chunk newChunk(coords);
                chunks.push_back(newChunk);

                chunkIndices.emplace(coords, chunks.size() - 1);

                bus.sendMessage<ChunkCreatedMessage>(ChunkCreatedMessage(&coords,&newChunk));
            }
        }
    }
}

void World::update()
{
}

void World::hehehe()
{
}
