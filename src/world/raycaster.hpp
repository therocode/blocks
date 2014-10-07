#pragma once
#include "../utilities/glm.hpp"

class World;
using VoxelCoord = glm::i64vec3;

class RayCaster
{
    public:
        static bool getVoxelAtRay(const World& world, const glm::vec3& position, const glm::vec3& direction, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock );
};
