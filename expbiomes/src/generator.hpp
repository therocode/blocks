#pragma once
#include <fea/util/noise.hpp>

class Generator
{
    public:
        Generator();
        void generateCoordinate(float x, float y, float& height, float& rain, float& temperature, float& biome);
    private:
        fea::Noise noise0;
        fea::Noise noise1;
        fea::Noise noise2;
        fea::Noise noise3;
        fea::Noise noise4;
        fea::Noise noise5;
        fea::Noise noise6;
        fea::Noise noise7;
};
