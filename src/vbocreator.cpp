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
                if(voxelTypes[x + y * chunkWidth + z * chunkWidthx2] != 0)
                {
                    //haha nothing
                    Rectangle front;
                    front.setPosition(0, x + chunkOffset.x,        y + 1.0f + chunkOffset.y, z + chunkOffset.z);
                    front.setPosition(1, x + chunkOffset.x,        y + chunkOffset.y,        z + chunkOffset.z);
                    front.setPosition(2, x + 1.0f + chunkOffset.x, y + chunkOffset.y,        z + chunkOffset.z);
                    front.setPosition(3, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y, z + chunkOffset.z);
                    front.setColor((float)(rand()%255) /255.f, (float)(rand()%255) /255.f, (float)(rand()%255) /255.f);
                    front.calculateNormal();
                    vbo.PushRectangle(front);
                    
                    Rectangle back;
                    back.setPosition(0, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    back.setPosition(1, x + 1.0f + chunkOffset.x, y + chunkOffset.y,        z + 1.0f + chunkOffset.z);
                    back.setPosition(2, x + chunkOffset.x,        y + chunkOffset.y,        z + 1.0f + chunkOffset.z);
                    back.setPosition(3, x + chunkOffset.x,        y + 1.0f + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    back.setColor((float)(rand()%255) /255.f, (float)(rand()%255) /255.f, (float)(rand()%255) /255.f);
                    back.calculateNormal();
                    vbo.PushRectangle(back);

                    Rectangle left;
                    left.setPosition(0, x + chunkOffset.x, y + 1.0f + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    left.setPosition(1, x + chunkOffset.x, y + chunkOffset.y,        z + 1.0f + chunkOffset.z);
                    left.setPosition(2, x + chunkOffset.x, y + chunkOffset.y,        z + chunkOffset.z);
                    left.setPosition(3, x + chunkOffset.x, y + 1.0f + chunkOffset.y, z + chunkOffset.z);
                    left.setColor((float)(rand()%255) /255.f, (float)(rand()%255) /255.f, (float)(rand()%255) /255.f);
                    left.calculateNormal();
                    vbo.PushRectangle(left);

                    Rectangle right;
                    right.setPosition(0, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y, z + chunkOffset.z);
                    right.setPosition(1, x + 1.0f + chunkOffset.x, y + chunkOffset.y,        z + chunkOffset.z);
                    right.setPosition(2, x + 1.0f + chunkOffset.x, y + chunkOffset.y,        z + 1.0f + chunkOffset.z);
                    right.setPosition(3, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    right.setColor((float)(rand()%255) /255.f, (float)(rand()%255) /255.f, (float)(rand()%255) /255.f);
                    right.calculateNormal();
                    vbo.PushRectangle(right);

                    Rectangle top;
                    top.setPosition(0, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    top.setPosition(1, x + chunkOffset.x,        y + 1.0f + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    top.setPosition(2, x + chunkOffset.x,        y + 1.0f + chunkOffset.y, z + chunkOffset.z);
                    top.setPosition(3, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y, z + chunkOffset.z);
                    top.setColor((float)(rand()%255) /255.f, (float)(rand()%255) /255.f, (float)(rand()%255) /255.f);
                    top.calculateNormal();
                    vbo.PushRectangle(top);

                    Rectangle bottom;
                    bottom.setPosition(0, x + 1.0f + chunkOffset.x, y + chunkOffset.y, z + chunkOffset.z);
                    bottom.setPosition(1, x + chunkOffset.x,        y + chunkOffset.y, z + chunkOffset.z);
                    bottom.setPosition(2, x + chunkOffset.x,        y + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    bottom.setPosition(3, x + 1.0f + chunkOffset.x, y + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    bottom.setColor((float)(rand()%255) /255.f, (float)(rand()%255) /255.f, (float)(rand()%255) /255.f);
                    bottom.calculateNormal();
                    vbo.PushRectangle(bottom);
                }
            }
        }
	}

	//After stuff has been added, you have to update the gpu vbo data.
	vbo.UpdateVBO();

	return vbo;
}
