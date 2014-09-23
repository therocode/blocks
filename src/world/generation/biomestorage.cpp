#include "biomestorage.hpp"
#include <algorithm>

const Biome* BiomeStorage::findBiome(float temperature, float rainfall, float height, float selector) const
{
    std::vector<const Biome*> possibleBiomes(mBiomes.size(), nullptr);

    if(mBiomes.size() > 0)
    {
        int32_t possibleBiomeAmount = 0;

        for(uint32_t i = 0; i < mBiomes.size(); i++)
        {
            const Biome& biome = mBiomes[i];

            if(biome.temperatureRange.isWithin(temperature) && biome.rainfallRange.isWithin(rainfall) && biome.heightRange.isWithin(height))
            {
                possibleBiomes[possibleBiomeAmount] = &biome;
                possibleBiomeAmount++;
            }
        }

        possibleBiomes.resize(possibleBiomeAmount);

        if(possibleBiomes.size() != 0)
        {
            size_t index = 0;

            index = possibleBiomes.size() * selector;

            return possibleBiomes[index];
        }
    }

    return nullptr;
}

void BiomeStorage::addBiome(const Biome& biome)
{
    mBiomes.push_back(biome);
}
