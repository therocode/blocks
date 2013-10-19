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
	glm::vec3 d = glm::vec3(dx, dy, dz);
	if(glm::length2(d) != 0)
	d = glm::normalize(d);
	d*= 0.1f;
	glm::vec3 p = glm::vec3(ox, oy, oz);
	
	float l = glm::sqrt(dx * dx + dy * dy + dz * dz);
	l *= 0.01;
	dx *= l;
	dy *= l;
	dz *= l;
	int steps = 0;
	uint16_t vtype = 0;
	while(steps < 900){
		
		glm::vec3 lp = glm::fract(p);
		float nd = 10.f;
		float ddot = glm::dot(glm::vec3(1.f, 0.f, 0.f), d);
		if(ddot>0){
			
		}
		
		vtype = getVoxelType(p.x, p.y, p.z);
		if(vtype != (uint16_t)0 ) break;
		p += d;
		
		steps ++;
	}
	return glm::floor(p) + glm::vec3(0.5f);
}

fea::WeakEntityPtr WorldInterface::spawnEntity(const std::string& scriptType, const glm::vec3& position)
{
    return mEntitySystem.spawnEntity(scriptType, position);
}

void WorldInterface::spawnEntityFromScriptHandle(const std::string& scriptType, const glm::vec3& position, asIScriptObject* obj)
{
    mEntitySystem.spawnEntityFromScriptHandle(scriptType, position, obj);    
}
