#include "world.h"
#include "chunkgenerator.h"
        
World::World(fea::MessageBus& messageBus) : bus(messageBus)
{
}

void World::initialise()
{
    ChunkGenerator generator; 

    for(int32_t x = -2; x < 3; x++)
    {
        for(int32_t y = -2; y < 3; y++)
        {
            for(int32_t z = -2; z < 3; z++)
            {
                ChunkCoordinate coords(x, y, z);

                Chunk newChunk(coords);
                chunks.push_back(newChunk);

                chunkIndices.emplace(coords, chunks.size() - 1);

                generator.generateChunkContent(newChunk);

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
