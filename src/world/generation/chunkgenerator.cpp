#include "chunkgenerator.hpp"
#include "../biome.hpp"

Chunk ChunkGenerator::generateChunk(const ChunkCoord& chunkCoordinate, const BiomeGrid& biomeData, const FieldMap& fieldData, const std::unordered_map<BiomeId, Biome>& biomes) const
{
    Chunk newChunk;

    VoxelCoord chunkVoxelCoord = WorldToVoxel::convert(ChunkToWorld::convert(chunkCoordinate));

    float chunkY = chunkVoxelCoord.y;

    VoxelTypeArray voxelData;

    size_t yIndex = 0;
    size_t zIndex = 0;


    for(int32_t y = 0; y < chunkWidth; y++)
    {
        yIndex = y * chunkWidthPow2;
        for(int32_t z = 0; z < chunkWidth; z++)
        {
            zIndex = z * chunkWidth;
            for(int32_t x = 0; x < chunkWidth; x++)
            {
                float worldY = (float)(chunkY + y);
                const Biome& biome = biomes.at(biomeData.get({x, y, z}));
                VoxelType biomeType = biome.mType;

                voxelData[zIndex + yIndex + x] = (worldY + 300.0f < (fieldData.at(1).get({x, y, z}) * 100.0f) * 5.0f) ? biomeType : 0;
            }
        }
    }

    newChunk.setVoxelData(voxelData);

    return newChunk;
}
