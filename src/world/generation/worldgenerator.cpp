#include "worldgenerator.h"

Chunk WorldGenerator::generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const
{
    Chunk newChunk;

    newChunk.setVoxelType(0, 0, 0, 3);
    newChunk.setVoxelType(1, 0, 0, 3);
    newChunk.setVoxelType(2, 0, 0, 3);
    newChunk.setVoxelType(3, 0, 0, 3);
    newChunk.setVoxelType(4, 0, 0, 3);
    newChunk.setVoxelType(5, 0, 0, 3);
    newChunk.setVoxelType(6, 0, 0, 3);
    newChunk.setVoxelType(7, 0, 0, 3);
    newChunk.setVoxelType(8, 0, 0, 3);
    newChunk.setVoxelType(9, 0, 0, 3);
    newChunk.setVoxelType(10, 0, 0, 3);
    newChunk.setVoxelType(11, 0, 0, 3);
    newChunk.setVoxelType(12, 0, 0, 3);
    newChunk.setVoxelType(12, 0, 0, 3);
    newChunk.setVoxelType(13, 0, 0, 3);
    newChunk.setVoxelType(14, 0, 0, 3);
    newChunk.setVoxelType(15, 0, 0, 3);

    return newChunk;
}
