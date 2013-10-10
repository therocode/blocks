#include "localchunkdeliverer.h"
#include "simplexnoise.h"

Chunk LocalChunkDeliverer::fetchChunk(const ChunkCoordinate& location) const
{
    Chunk newChunk(location);

    VoxelTypeArray& types = newChunk.getVoxelTypes();

    for(int z = 0; z < chunkWidth; z++)
    {
        for(int y = 0; y < chunkWidth; y++)
        {
            for(int x = 0; x < chunkWidth; x++)
            {
                float noiseXPos = ((float)(x + location.x * chunkWidth)) / 14.0f;
                float noiseYPos = ((float)(y + location.y * chunkWidth)) / 14.0f;
                float noiseZPos = ((float)(z + location.z * chunkWidth)) / 14.0f;
                float noise = raw_noise_3d(noiseXPos, noiseYPos, noiseZPos);
                if(noise < -0.5f)
                {
                    types[x + y * chunkWidth + z * chunkWidthx2] = 1;
                }
                else if(noise < 0.0f)
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

    return newChunk;
}
