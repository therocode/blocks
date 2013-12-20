#include "worldgenerator.h"
#include <iostream>

Chunk WorldGenerator::generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const
{
    Chunk newChunk;

    float chunkY = chunkCoordinate.y * chunkWidth;

    for(int32_t x = 0; x < chunkWidth; x++)
    for(int32_t y = 0; y < chunkWidth; y++)
    for(int32_t z = 0; z < chunkWidth; z++)
    {
        float worldY = (float)(chunkY + y);
        if(worldY < 0.0f)
            newChunk.setVoxelType(x, y, z, 1);
        else
            newChunk.setVoxelType(x, y, z, 0);
    }

    return newChunk;
}
