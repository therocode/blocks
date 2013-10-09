#include "vbocreator.h"
#include <vector>
#include <iostream>

ChunkVBO VBOCreator::generateChunkVBO(const Chunk& chunk) const
{
	ChunkVBO vbo;

    const ChunkCoordinate location = chunk.getLocation();

    glm::vec3 chunkOffset(location.x * chunkWidth, location.y * chunkWidth, location.z * chunkWidth);

	const VoxelTypeArray& voxelTypes = chunk.getVoxelTypes();

    for(int z = 0; z < chunkWidth; z++)
    {
        for(int y = 0; y < chunkWidth; y++)
        {
            for(int x = 0; x < chunkWidth; x++)
            {
                //haha nothing
                Rectangle r;
                r.setPosition(0, x + chunkOffset.x,        y + 1.0f + chunkOffset.y , z + chunkOffset.z);
                r.setPosition(1, x + chunkOffset.x,        y + chunkOffset.y,         z + chunkOffset.z);
                r.setPosition(2, x + 1.0f + chunkOffset.x, y + chunkOffset.y,         z + chunkOffset.z);
                r.setPosition(3, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y,  z + chunkOffset.z);
                r.setColor((float)(rand()%255) /255.f, (float)(rand()%255) /255.f, (float)(rand()%255) /255.f);
                r.calculateNormal();
                vbo.PushRectangle(r);
            }
        }
	}

	//After stuff has been added, you have to update the gpu vbo data.
	vbo.UpdateVBO();

	return vbo;
}
