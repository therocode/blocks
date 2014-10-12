#pragma once
#include "../utilities/glm.hpp"
#include "worlddefines.hpp"
#include "chunkmap.hpp"

class RayCaster
{
    public:
        static bool getVoxelAtRay(const ChunkMap& world, const glm::vec3& position, const glm::vec3& direction, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock );
};
