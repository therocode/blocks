#include "worldinterface.h"
#include "rendering/renderer.h"
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
	glm::vec3 bp = glm::fract(glm::vec3(ox, oy, oz));
	int ip[3] = {(int)ox, (int)oy, (int)oz};
	//glm::vec3 ip = glm::vec3((int), (int)oy, (int)oz);
	
	glm::vec3 d = glm::vec3(dx, dy, dz);
	if(glm::length2(d) != 0)
		d = glm::normalize(d);
	else
		return glm::vec3(0, 9999,0);
	
	glm::vec3 p = glm::vec3(ox, oy, oz);
	
	int steps = 0;
	uint16_t vtype = 0;
	glm::vec3 bounds = glm::vec3(0,0,0);
	if(dx > 0)bounds.x = 1.0f;
	if(dy > 0)bounds.y = 1.0f;
	if(dz > 0)bounds.z = 1.0f;
	float ix, iy, iz;
	// printf("o: %i, %i, %i\n", ip[0], ip[1], ip[2]);
	while(steps < 256){//Able to look 256 blocks away!
		//glm::vec3 lp = glm::fract(p);
		glm::vec3 distInBlock = bounds - bp;

		glm::vec3 poop = distInBlock / d;
		
		float mind = poop.x;
		
		if(mind > poop.y){
			mind = poop.y;
		}
		if(mind > poop.z){
			mind = poop.z;
		}
		
		float lengthToNextBlock = 0.1f;
		lengthToNextBlock = mind + 0.01f;
		
		vtype = getVoxelType(ip[0], ip[1], ip[2]);
		if(vtype != (uint16_t)0 ) 
			break;
		
		bp += d * lengthToNextBlock;
		for(int i = 0; i < 3; i ++){
			if(bp[i] >= 1.f){
				bp[i] -= 1.f;
				ip[i] ++;
			}else if(bp[i] <= 0.f){
				bp[i] += 1.f;
				ip[i] --;
			}
		}
		steps ++;
	}
	if(steps == 256){ return glm::vec3(0, 9999, 0); }
	return glm::vec3(ip[0] - (ip[0] < 0), ip[1] - (ip[1] < 0), ip[2] - (ip[2] < 0)) + glm::vec3(0.5f);
	// return glm::vec3((int)p.x - (p.x<0),(int)p.y - (p.y<0),(int)p.z - (p.z<0)) + glm::vec3(0.5f);
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
