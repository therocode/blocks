#pragma once
#include "dimension.h"
#include "entitysystem.h"

class WorldInterface
{
    public:
        WorldInterface(Dimension& dimension, EntitySystem& entitySystem);
        VoxelType getVoxelType(float x, float y, float z) const; 
        VoxelType getVoxelType(const glm::vec3& position) const; 
		glm::vec3 getVoxelAtRay(const glm::vec3& position, const glm::vec3& direction) const;
		glm::vec3 getVoxelAtRay(float ox, float oy, float oz, float dx, float dy, float dz) const;
    private:
        Dimension& mDimension;
        EntitySystem& mEntitySystem;
};
