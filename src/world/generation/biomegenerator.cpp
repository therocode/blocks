#include "biomegenerator.hpp"

BiomeGenerator::BiomeGenerator(IntensityMap* h, IntensityMap* r, IntensityMap* t, IntensityMap* b, BiomeStorage& s) : heightmap(h), rainfall(r), temperature(t), biomeSelector(b), storage(s)
{
}

VoxelTypeMap generateTypeMap()
{
    for(int x = 0; x < partSize; x++)
    {
        for(int y = 0; y < partSize; y++)
        {
            float temp = temperature->getUnit(x, y);
            float rain = rainfall->getUnit(x, y);
            float selector = biomeSelector->getUnit(x, y);
            float height = heightmap->getUnit(x, y);

            Biome* biome = storage.getBiome(temp, rain, height, selector);

            if(biome)
            {
                if(height < 0.7f)
                    height = 0.7f;

                texture.setPixel(x, y, biome->r * height, biome->g * height, biome->b * height);
            }
            else
            {
                texture.setPixel(x, y, 0.0f, 0.0f, 0.0f);
            }
        }
    }
}
