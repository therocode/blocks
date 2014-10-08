#pragma once
#include "../utilities/glm.hpp"
#include "worldconstants.hpp"
#include "voxelstorage.hpp"

class RayCaster
{
    public:
        static bool getVoxelAtRay(const VoxelStorage& world, const glm::vec3& position, const glm::vec3& direction, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock );
};
