#pragma once
#include <vector>

struct RawModel
{
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<std::vector<uint32_t>> indices;
};
