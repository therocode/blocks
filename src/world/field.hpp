#pragma once
#include <string>

using FieldGrid = InterpolationGrid3D<float>;

struct Field
{
    std::string name;
    uint32_t downSamplingFactor;
};
