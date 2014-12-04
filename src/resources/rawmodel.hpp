#pragma once
#include <vector>

struct RawModel
{
    std::vector<float> positions;
    std::vector<std::vector<uint32_t>> indices;
};
