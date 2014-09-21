#include "chunkgenerator.hpp"
#include "biome.hpp"

Chunk ChunkGenerator::generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const
{
    Chunk newChunk(chunkCoordinate);

    VoxelCoord chunkVoxelCoord = worldToVoxel(chunkToWorld(chunkCoordinate));
    //std::cout << chunkVoxelCoord.z << " is z \n";
    RegionVoxelCoord regionStart = voxelToRegionVoxel(chunkVoxelCoord);
    RegionVoxelCoord regionCoord = regionStart;

    float chunkY = chunkVoxelCoord.y;

    VoxelTypeArray voxelData;

    size_t yIndex = 0;
    size_t zIndex = 0;

    for(int32_t y = 0; y < chunkWidth; y++)
    {
        yIndex = y * chunkWidthPow2;
        regionCoord.y = regionStart.y;
        for(int32_t z = 0; z < chunkWidth; z++)
        {
            zIndex = z * chunkWidth;
            regionCoord.x = regionStart.x;
            for(int32_t x = 0; x < chunkWidth; x++)
            {
                float worldY = (float)(chunkY + y);
                float threshold = region.getHeightmap().getUnit(regionCoord.x, regionCoord.y) * 100.f;
                const Biome* biome = region.getBiome(regionCoord);
                VoxelType biomeType = biome->mType;

                //bool cave = mNoise.simplex3D((((float)x) + chunkWidth * chunkCoordinate.x) * 0.02f, (((float)y) + chunkWidth * chunkCoordinate.y) * 0.02f, (((float)z) + chunkWidth * chunkCoordinate.z) * 0.02f) > 0.0f;
                bool cave = false;

                voxelData[zIndex + yIndex + x] = ((worldY < threshold) && !cave) ? biomeType : 0;

                regionCoord.x++;
            }
            regionCoord.y++;
        }
    }

    newChunk.setVoxelData(voxelData);

    return newChunk;
}
