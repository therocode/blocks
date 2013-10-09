#include "chunkgenerator.h"

void ChunkGenerator::generateChunkContent(Chunk& chunk) const
{
    VoxelTypeArray& types = chunk.getVoxelTypes();

    for(int z = 0; z < chunkWidth; z++)
    {
        for(int y = 0; y < chunkWidth; y++)
        {
            for(int x = 0; x < chunkWidth; x++)
            {
                if(rand() % 5 == 0)
                {
                    types[x + y * chunkWidth + z * chunkWidthx2] = 1;
                }
                else
                {
                    types[x + y * chunkWidth + z * chunkWidthx2] = 0;
                }
            }
        }
    }
}
