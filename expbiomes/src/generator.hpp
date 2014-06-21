#pragma once
#include <fea/util/noise.hpp>

class Generator
{
    public:
        void generateCoordinate(float x, float y, float& height, float& rain, float& temperature, float& biome);
    private:
        fea::Noise noise;
};
