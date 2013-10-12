#pragma once
#include "dimension.h"
#include "entitysystem.h"

class WorldInterface
{
    public:
        WorldInterface(Dimension& dimension, EntitySystem& entitySystem);
        VoxelType getVoxelType(float x, float y, float z) const; 
        VoxelType getVoxelType(const glm::vec3& position) const; 
    private:
        Dimension& mDimension;
        EntitySystem& mEntitySystem;
};
