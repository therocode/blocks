#include "worldgenerator.h"
#include "../region.h"
#include <iostream>

Chunk WorldGenerator::generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const
{
    Chunk newChunk;

    VoxelCoord chunkVoxelCoord = worldToVoxel(chunkToWorld(chunkCoordinate));
    VoxelCoord voxelCoord;
    RegionVoxelCoord regionCoord;

    float chunkY = chunkToWorld(chunkCoordinate).y;

    std::cout << "chunk is " << glm::to_string((glm::ivec3)chunkCoordinate) << "\n";

    voxelCoord.x = chunkVoxelCoord.x;
    for(int32_t x = 0; x < chunkWidth; x++)
    {
        voxelCoord.y = chunkVoxelCoord.y;
        for(int32_t y = 0; y < chunkWidth; y++)
        {
            voxelCoord.z = chunkVoxelCoord.z;
            for(int32_t z = 0; z < chunkWidth; z++)
            {
    if(chunkCoordinate == ChunkCoord(1,0,3))
    std::cout << "voxel z is " << glm::to_string((glm::ivec3)voxelCoord) << "\n";
                //std::cout << "voxel coord for chunk is " << voxelCoord.x << " " << voxelCoord.y << " " << voxelCoord.z << "\n";
                regionCoord = voxelToRegionVoxel(voxelCoord);
                //std::cout << "region coord: " << regionCoord.x << " " << regionCoord.y << "\n";
                float worldY = (float)(chunkY + y) + region.getHeightmap().getUnit(regionCoord.x, regionCoord.y) * 100.0f;
                if(worldY < 0.0f)
                    newChunk.setVoxelType(x, y, z, 1);
                else
                    newChunk.setVoxelType(x, y, z, 0);
                
                voxelCoord.z++;
            }
            voxelCoord.y++;
        }
        voxelCoord.x++;
    }

    return newChunk;
}
