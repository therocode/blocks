#pragma once
#include "biome.hpp"

class BiomeStorage
{
    public:
        Biome* getBiome(float temperature, float rainfall, float height, float selector);
        void addBiome(Biome* biome);
    private:
        std::vector<Biome*> biomes; 
};
