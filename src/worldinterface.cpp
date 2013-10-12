#include "worldinterface.h"
#include <iostream>

WorldInterface::WorldInterface(Dimension& dimension, EntitySystem& entitySystem)
    :   mDimension(dimension),
        mEntitySystem(entitySystem)
{
    
}

VoxelType WorldInterface::getVoxelType(float x, float y, float z) const
{
    ChunkCoordinate chunkCoordinate = worldToChunk(x, y, z);
    VoxelCoordinate voxelCoordinate = worldToVoxel(x, y, z);

    std::cout << "now i am at position " << x << " " << y << " " << z << "\n";
    std::cout << "will check chunk at " << chunkCoordinate.x << " " << chunkCoordinate.y << " "  << chunkCoordinate.z << "\n";

    const Landscape& landscape = mDimension.getLandscape();

    if(landscape.chunkIsLoaded(chunkCoordinate))
    {
    std::cout << "will check voxel at " << voxelCoordinate.x << " " << voxelCoordinate.y << " "  << voxelCoordinate.z << "\n";
        return landscape.getChunk(chunkCoordinate).getVoxelType(voxelCoordinate);
    }
    else
    {
        return 0;
    }
}

VoxelType WorldInterface::getVoxelType(const glm::vec3& position) const
{
    return getVoxelType(position.x, position.y, position.z);
}
