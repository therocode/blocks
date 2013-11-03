#include "worldinterface.h"

	WorldInterface::WorldInterface(Dimension& dimension, EntitySystem& entitySystem)
:   mDimension(dimension),
	mEntitySystem(entitySystem)
{

}

VoxelType WorldInterface::getVoxelType(float x, float y, float z) const
{
	ChunkCoordinate chunkCoordinate = worldToChunk(x, y, z);
	VoxelCoordinate voxelCoordinate = worldToChunkVoxel(x, y, z);

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
	//d*= 0.1f;
	glm::vec3 p = glm::vec3(ox, oy, oz);
	
	float l = glm::sqrt(dx * dx + dy * dy + dz * dz);
	l *= 0.01;
	dx *= l;
	dy *= l;
	dz *= l;
	int steps = 0;
	uint16_t vtype = 0;
	glm::vec3 bounds = glm::vec3(0,0,0);
	if(dx > 0)bounds.x = 1.0f;
	if(dy > 0)bounds.y = 1.0f;
	if(dz > 0)bounds.z = 1.0f;
	
	while(steps < 256){//Able to look 256 blocks away!
		
		glm::vec3 lp = glm::fract(p);
		float nd = 10.f;
		glm::vec3 distInBlock = bounds - lp;

		glm::vec3 poop = distInBlock / d;
		
		float mind = poop.x;
		
		if(mind > poop.y){
			mind = poop.y;
		}
		if(mind > poop.z){
			mind = poop.z;
		}
		
		float lengthToNextBlock = 0.1f;
		lengthToNextBlock = mind + 0.001f;
		
		vtype = getVoxelType(p.x, p.y, p.z);
		if(vtype != (uint16_t)0 ) break;
		p += d * lengthToNextBlock;
		
		steps ++;
	}
	return glm::floor(p) + glm::vec3(0.5f);
}

fea::WeakEntityPtr WorldInterface::spawnEntity(const std::string& scriptType, const glm::vec3& position)
{
    return mEntitySystem.spawnEntity(scriptType, position);
}

size_t WorldInterface::spawnEntityFromScriptHandle(const std::string& scriptType, const glm::vec3& position, asIScriptObject* obj)
{
    return mEntitySystem.spawnEntityFromScriptHandle(scriptType, position, obj);    
}

const ChunkMap& WorldInterface::getChunkList() const
{
    return mDimension.getLandscape().getChunkList();
}
