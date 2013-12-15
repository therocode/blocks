#include "localchunkprovider.h"
#include "utilities/simplexnoise.h"

LocalChunkProvider::LocalChunkProvider() : mNoise(11)
{
}

Chunk LocalChunkProvider::fetchChunk(const ChunkCoordinate& location) const
{
    Chunk newChunk(location);

    VoxelTypeArray types;

    for(int32_t z = 0; z < chunkWidth; z++)
    {
        for(int32_t y = 0; y < chunkWidth; y++)
        {
            for(int32_t x = 0; x < chunkWidth; x++)
            {
                int currentBlock = x + z * chunkWidth + y * chunkWidthx2;
                float noiseXPos = ((float)(x + location.x * (int32_t)chunkWidth)) / 14.0f;
                float noiseYPos = ((float)(y + location.y * (int32_t)chunkWidth)) / 14.0f;
                float noiseZPos = ((float)(z + location.z * (int32_t)chunkWidth)) / 14.0f;

                float noise = mNoise.simplex3D(noiseXPos, noiseYPos, noiseZPos);
                float noiseHeight = ((mNoise.simplexOctave2D(noiseXPos * 0.1f, noiseZPos * 0.1f, 1.0f, 4) ) * 50.f );

                if(noiseHeight > (40 - (location.y * (int32_t)chunkWidth + y)) )
                {
                    if(noise < 0.0f && (40 - (location.y * (int32_t)chunkWidth + y)) > noiseHeight - 1)
                        noise = -0.9f;
                    else
                        noise = 1.0f;
                }
                if(noise < -0.5f)
                {
                    types[currentBlock] = 2;
                }
                else if(noise < 0.5f)
                {
                    types[currentBlock] = 2;
                }
                else
                {
                    types[currentBlock] = 0;
                }
                if(noiseYPos + noiseHeight * 1.f < -1 && types[currentBlock] != 0)
                {
                    types[currentBlock] = 15;
                }
                if(types[currentBlock] != 0)
                {
                    if(40 - ((location.y * (int32_t)chunkWidth + y)) == (int)noiseHeight)
                        types[currentBlock] = 1;
                    float s = 0.5f;
                    noiseYPos *= s;
                    noiseXPos *= s;
                    noiseZPos *= s;
                    if(mNoise.simplex3D(noiseYPos, noiseXPos, noiseZPos) > 0.9)
                    {
                        types[currentBlock] = 19;
                    }
                }
				float poo = 0;//glm::sin(glm::pow(noiseYPos, 2.f));
				float len = glm::length(glm::vec2(0, -4) - glm::vec2(noiseXPos * 14 , noiseZPos * 14 )) + poo;
				if(len < 4 && noiseYPos < 0)
				{
					types[currentBlock] = 8;
				}
				if(len<2)
				{
					types[currentBlock] = 0;
				}
			}
        }
    }

    newChunk.setVoxelData(types);
    //newChunk.setVoxelType(0,15,0,8);
    return newChunk;
}
