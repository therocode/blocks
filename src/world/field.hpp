#pragma once

enum NoiseType { SIMPLEX, VORONOI };

using FieldGrid = InterpolationGrid3D<float>;

struct Field
{
    uint32_t id;
    NoiseType noiseType;
    bool isSelector;
};
