#include "worldgenerator.h"
#include "../region.h"
#include <iostream>

Chunk WorldGenerator::generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const
{
    Chunk newChunk(chunkCoordinate);

    VoxelCoord chunkVoxelCoord = worldToVoxel(chunkToWorld(chunkCoordinate));
    std::cout << chunkVoxelCoord.z << " is z \n";
    VoxelCoord voxelCoord;
    RegionVoxelCoord regionCoord;

    float chunkY = chunkVoxelCoord.y;

    //std::cout << "chunk is " << glm::to_string((glm::ivec3)chunkCoordinate) << "\n";

    for(int32_t y = 0; y < chunkWidth; y++)
    {
        voxelCoord.x = chunkVoxelCoord.x;
        for(int32_t x = 0; x < chunkWidth; x++)
        {
            voxelCoord.z = chunkVoxelCoord.z;
            for(int32_t z = 0; z < chunkWidth; z++)
            {
                //std::cout << "voxel coord for chunk is " << voxelCoord.x << " " << voxelCoord.y << " " << voxelCoord.z << "\n";
                regionCoord = voxelToRegionVoxel(voxelCoord);
                //if(chunkCoordinate == ChunkCoord(1,0,3))
                //std::cout << "region coord: " << regionCoord.x << " " << regionCoord.y << "\n";
                float worldY = (float)(chunkY + y) + region.getHeightmap().getUnit(regionCoord.x, regionCoord.y) * 100.f;
                if(worldY < 0.0f)
                {
                    RegionCoord region = voxelToRegion(voxelCoord);
                    if(region == RegionCoord(0,0))
                        newChunk.setVoxelType(x, y, z, 2);
                    else if(region == RegionCoord(-1, 0))
                        newChunk.setVoxelType(x, y, z, 1);
                    else if(region == RegionCoord(0, -1))
                        newChunk.setVoxelType(x, y, z, 4);
                    else if(region == RegionCoord(-1, -1))
                        newChunk.setVoxelType(x, y, z, 1);
                }
                else
                    newChunk.setVoxelType(x, y, z, 0);

                voxelCoord.z++;
            }
            voxelCoord.x++;
        }
    }

    return newChunk;
}
