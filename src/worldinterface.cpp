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

	const Landscape& landscape = mDimension.getLandscape();

	if(landscape.chunkIsLoaded(chunkCoordinate))
	{
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

glm::vec3 WorldInterface::getVoxelAtRay(const glm::vec3& position, const glm::vec3& direction) const
{
	return getVoxelAtRay(position.x, position.y, position.z, direction.x, direction.y, direction.z);
}

glm::vec3 WorldInterface::getVoxelAtRay(float ox, float oy, float oz, float dx, float dy, float dz) const
{
	float l = glm::sqrt(dx * dx + dy * dy + dz * dz);
	l *= 0.01;
	dx *= l;
	dy *= l;
	dz *= l;
	float x = ox, y = oy, z = oz;
	int steps = 0;
	uint16_t vtype = 0;
	while(vtype < (uint16_t)1 && steps < 200){
		vtype = getVoxelType(x - 1.f, y-1.f, z-1.f);
		printf("voxel type: %i\n", vtype);
		x += dx;	
		y += dy;	
		z += dz;	
		steps ++;
	}
	printf("steps: %i, step length: %f\n", steps, l);
	return glm::floor(glm::vec3(x, y, z)) + glm::vec3(0.5f);
}
