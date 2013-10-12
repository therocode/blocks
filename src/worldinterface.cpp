#include "worldinterface.h"

WorldInterface::WorldInterface(Dimension& dimension, EntitySystem& entitySystem)
    :   mDimension(dimension),
        mEntitySystem(entitySystem)
{
    
}

VoxelType WorldInterface::getVoxelType(float x, float y, float z) const
{
    ChunkCoordinate chunkCoordinate = worldToChunk(x, y, z);
    VoxelCoordinate voxelCoordinate = worldToVoxel(x, y, z);

    return 0;
}

VoxelType WorldInterface::getVoxelType(const glm::vec3& position) const
{
    return getVoxelType(position.x, position.y, position.z);
}
