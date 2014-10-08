#include "chunkgenerator.hpp"
#include "biome.hpp"
#include "biomestorage.hpp"
#include "../regiondatafragment.hpp"

Chunk ChunkGenerator::generateChunk(const ChunkCoord& chunkCoordinate, const RegionDataFragment& regionFragment, const BiomeStorage& biomes) const
{
    Chunk newChunk(chunkCoordinate);

    VoxelCoord chunkVoxelCoord = WorldToVoxel::convert(ChunkToWorld::convert(chunkCoordinate));
    //std::cout << chunkVoxelCoord.z << " is z \n";

    float chunkY = chunkVoxelCoord.y;

    VoxelTypeArray voxelData;

    size_t yIndex = 0;
    size_t zIndex = 0;

    //for(int32_t y = 0; y < chunkWidth; y++)
    //{
    //    yIndex = y * chunkWidthPow2;
    //    regionCoord.y = regionStart.y;
    //    for(int32_t z = 0; z < chunkWidth; z++)
    //    {
    //        zIndex = z * chunkWidth;
    //        regionCoord.x = regionStart.x;
    //        for(int32_t x = 0; x < chunkWidth; x++)
    //        {
    //            float worldY = (float)(chunkY + y);
    //            float height = regionFragment.getHeightData().getUnit(x, z);
    //            float rain = regionFragment.getRainData().getUnit(x, z);
    //            float temperature = regionFragment.getTemperatureData().getUnit(x, z);
    //            float biomeSelector = regionFragment.getBiomeSelectorData().getUnit(x, z);
    //            float threshold = height * 100.f;
    //            const Biome* biome = biomes.findBiome(height, rain, temperature, biomeSelector);
    //            VoxelType biomeType = biome->mType;

    //            bool cave = false;

    //            voxelData[zIndex + yIndex + x] = ((worldY < threshold) && !cave) ? biomeType : 0;

    //            regionCoord.x++;
    //        }
    //        regionCoord.y++;
    //    }
    //}

    //newChunk.setVoxelData(voxelData);

    return newChunk;
}
