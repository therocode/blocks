#include "localchunkprovider.h"
#include "simplexnoise.h"

Chunk LocalChunkProvider::fetchChunk(const ChunkCoordinate& location) const
{
    Chunk newChunk(location);

    VoxelTypeArray& types = newChunk.getVoxelTypes();

    for(int32_t z = 0; z < chunkWidth; z++)
    {
        for(int32_t y = 0; y < chunkWidth; y++)
        {
            for(int32_t x = 0; x < chunkWidth; x++)
            {
                float noiseXPos = ((float)(x + location.x * (int32_t)chunkWidth)) / 14.0f;
                float noiseYPos = ((float)(y + location.y * (int32_t)chunkWidth)) / 14.0f;
                float noiseZPos = ((float)(z + location.z * (int32_t)chunkWidth)) / 14.0f;

                float noise = raw_noise_3d(noiseXPos, noiseYPos, noiseZPos);
				float noiseHeight = ((raw_noise_2d(noiseXPos * 0.5f, noiseZPos * 0.5f) +1.f) * 5.f ); 
				if(noiseHeight > (40 - (location.y * (int32_t)chunkWidth + y)) )
				{
                	if(noise < 0.0f && (40 - (location.y * (int32_t)chunkWidth + y)) > noiseHeight - 1) noise = -0.9f;  
					else noise = 1.0f; 
				}
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
