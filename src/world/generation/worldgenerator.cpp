#include "worldgenerator.h"
#include "../region.h"
#include <iostream>

Chunk WorldGenerator::generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const
{
    Chunk newChunk(chunkCoordinate);

    VoxelCoord chunkVoxelCoord = worldToVoxel(chunkToWorld(chunkCoordinate));
    //std::cout << chunkVoxelCoord.z << " is z \n";
    VoxelCoord voxelCoord;
    RegionVoxelCoord regionCoord;

    float chunkY = chunkVoxelCoord.y;

    //std::cout << "chunk is " << glm::to_string((glm::ivec3)chunkCoordinate) << "\n";

    //must probably be something with the for looping and/or the conversion functions; all start coordinates are unique anyway and regionCoord.x seems to be the culprit

    VoxelTypeArray voxelData;

    size_t yIndex = 0;
    size_t zIndex = 0;

    for(int32_t y = 0; y < chunkWidth; y++)
    {
        yIndex = y * chunkWidthx2;
        voxelCoord.z = chunkVoxelCoord.z;
        for(int32_t z = 0; z < chunkWidth; z++)
        {
            zIndex = z * chunkWidth;
            voxelCoord.x = chunkVoxelCoord.x;
            for(int32_t x = 0; x < chunkWidth; x++)
            {
                //std::cout << "voxel coord for chunk is " << voxelCoord.x << " " << voxelCoord.y << " " << voxelCoord.z << "\n";
                regionCoord = voxelToRegionVoxel(voxelCoord);
                //if(chunkCoordinate == ChunkCoord(1,0,3))
                //std::cout << "region coord: " << regionCoord.x << " " << regionCoord.y << "\n";
                float worldY = (float)(chunkY + y);
                float threshold =  region.getHeightmap().getUnit(regionCoord.x, regionCoord.y) * 100.f;
                VoxelType biomeType = region.getBiomeTypes().getUnit(regionCoord.x, regionCoord.y);

                if(worldY < threshold)
                {
                    voxelData[zIndex + yIndex + x] = biomeType;
                }
                else
                    voxelData[zIndex + yIndex + x] = 0;

                voxelCoord.x++;
            }
            voxelCoord.z++;
        }
    }

    newChunk.setVoxelData(voxelData);

    return newChunk;
}
