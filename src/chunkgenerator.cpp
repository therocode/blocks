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
                uint32_t random = rand() % 10;
                if(random == 0)
                {
                    types[x + y * chunkWidth + z * chunkWidthx2] = 1;
                }
                else if(random == 1)
                {
                    types[x + y * chunkWidth + z * chunkWidthx2] = 2;
                }
                else
                {
                    types[x + y * chunkWidth + z * chunkWidthx2] = 0;
                }
            }
        }
    }
}
