#pragma once
#include <fea/util.hpp>
#include "biomedefines.hpp"
#include "field.hpp"

class FieldGenerator
{
    public:
        void fill(const BiomeRegionCoord& coordinate, FieldGrid& grid, NoiseType type);
    private:
        fea::Noise mNoise;
};
