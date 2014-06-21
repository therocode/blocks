#include "biomestorage.hpp"

Biome* BiomeStorage::getBiome(float temperature, float rainfall, float height, float selector)
{
    std::vector<size_t> possibleBiomes;

    if(biomes.size() > 0)
    {
        for(size_t i = 0; i < biomes.size(); i++)
        {
            if(biomes[i]->temperatureRange.isWithin(temperature) && biomes[i]->rainfallRange.isWithin(rainfall) && biomes[i]->heightRange.isWithin(height))
                possibleBiomes.push_back(i);
        }

        //for(uint32_t i = 0; i < possibleBiomes.size(); i++)
        //{
        //    if(possibleBiomes[i] == nullptr)
        //    {
        //        possibleBiomes.erase(possibleBiomes.begin() + i);
        //        i--;
        //    }
        //}

        if(possibleBiomes.size() != 0)
        {
            size_t index = 0;

            index = possibleBiomes.size() * selector;

            return biomes[possibleBiomes[index]].get();
        }
    }

    return nullptr;
}

void BiomeStorage::addBiome(Biome* biome)
{
    biomes.push_back(std::unique_ptr<Biome>(biome));
}
