#include "biomestorage.h"
#include <algorithm>

Biome* BiomeStorage::getBiome(float temperature, float rainfall, float height, float selector)
{
    std::vector<Biome*> possibleBiomes(biomes.size(), nullptr);

    if(biomes.size() > 0)
    {
        std::copy_if(biomes.begin(), biomes.end(), possibleBiomes.begin(), [=] (Biome* biome) {return biome->temperatureRange.isWithin(temperature) && biome->rainfallRange.isWithin(rainfall) && biome->heightRange.isWithin(height);});

        for(uint32_t i = 0; i < possibleBiomes.size(); i++)
        {
            if(possibleBiomes[i] == nullptr)
            {
                possibleBiomes.erase(possibleBiomes.begin() + i);
                i--;
            }
        }

        if(possibleBiomes.size() != 0)
        {
            size_t index = 0;

            index = possibleBiomes.size() * selector;

            return possibleBiomes[index];
        }
    }

    return nullptr;
}

void BiomeStorage::addBiome(Biome* biome)
{
    biomes.push_back(biome);
}
