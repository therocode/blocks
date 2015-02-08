#pragma once
#include "model.hpp"

struct ChunkModel
{
    Model model;
    std::vector<uint32_t> textureIndices;
};
