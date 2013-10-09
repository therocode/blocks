#include "vbocreator.h"
#include <vector>
#include <iostream>

ChunkVBO VBOCreator::generateChunkVBO(const Chunk& chunk) const
{
	ChunkVBO vbo;

    const ChunkCoordinate location = chunk.getLocation();

    glm::vec3 chunkOffset(location.x * chunkWidth, location.y * chunkWidth, location.z * chunkWidth);

	const VoxelTypeArray& voxelTypes = chunk.getVoxelTypes();

	for(int i = 0; i < 10; i ++){
		for(int y = 0; y < 10; y++){
			//haha nothing
			Rectangle r;
			r.setPosition(0, i + chunkOffset.x,     i + 1 + y + chunkOffset.y , 2 + chunkOffset.z);
			r.setPosition(1, i + chunkOffset.x,     i + y      + chunkOffset.y, 2 + chunkOffset.z);
			r.setPosition(2, i + 1 + chunkOffset.x, i + y       + chunkOffset.y,2 + chunkOffset.z);
			r.setPosition(3, i + 1 + chunkOffset.x, i + 1 + y   + chunkOffset.y,2 + chunkOffset.z);
			r.setColor((float)(rand()%255) /255.f, (float)(rand()%255) /255.f, (float)(rand()%255) /255.f);
			r.calculateNormal();
			vbo.PushRectangle(r);
		}
	}

	//After stuff has been added, you have to update the gpu vbo data.
	vbo.UpdateVBO();

	return vbo;
}
