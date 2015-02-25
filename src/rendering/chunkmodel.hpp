#pragma once
#include "model.hpp"
#include "../world/worlddefines.hpp"

struct ChunkModel
{
    Model model;
    std::vector<uint32_t> textureIndices;
};
