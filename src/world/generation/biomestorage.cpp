#include "biomestorage.hpp"
#include <algorithm>

const Biome* BiomeStorage::findBiome(float temperature, float rainfall, float height, float selector) const
{
    std::vector<const Biome*> possibleBiomes(biomes.size(), nullptr);

    if(biomes.size() > 0)
    {
        for(const auto& biome : biomes)
        {
            if(biome.temperatureRange.isWithin(temperature) && biome.rainfallRange.isWithin(rainfall) && biome.heightRange.isWithin(height))
                possibleBiomes.push_back(&biome);
        }

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

void BiomeStorage::addBiome(Biome biome)
{
    biomes.push_back(biome);
}
