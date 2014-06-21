#pragma once
#include "biome.hpp"
#include <vector>
#include <memory>

class BiomeStorage
{
    public:
        Biome* getBiome(float temperature, float rainfall, float height, float selector);
        void addBiome(Biome* biome);
    private:
        std::vector<std::unique_ptr<Biome>> biomes; 
};

