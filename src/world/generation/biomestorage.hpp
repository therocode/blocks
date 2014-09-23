#pragma once
#include "biome.hpp"

class BiomeStorage
{
    public:
        const Biome* findBiome(float temperature, float rainfall, float height, float selector) const;
        void addBiome(const Biome& biome);
    private:
        std::vector<Biome> mBiomes; 
};
