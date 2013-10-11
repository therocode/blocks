#include "vbocreator.h"
#include <vector>
#include <iostream>

ChunkVBO VBOCreator::generateChunkVBO(const Chunk& chunk) const
{
	ChunkVBO vbo;

    const ChunkCoordinate location = chunk.getLocation();

    glm::vec3 chunkOffset(location.x * (float)chunkWidth, location.y * (float)chunkWidth, location.z * (float)chunkWidth);

	const VoxelTypeArray& voxelTypes = chunk.getVoxelTypes();

    glm::uvec2 textureLocation;

    std::cout << "generating a vbo for a chunk with location: " << chunkOffset.x << " " << chunkOffset.y << " " << chunkOffset.z << "\n";

    for(int z = 0; z < chunkWidth; z++)
    {
        for(int y = 0; y < chunkWidth; y++)
        {
            for(int x = 0; x < chunkWidth; x++)
            {
                uint16_t type = voxelTypes[x + y * chunkWidth + z * chunkWidthx2];
                if(type != 0)
                {
                    if(type == 1)
                        textureLocation = glm::uvec2(0, 0);
                    if(type == 2)
                        textureLocation = glm::uvec2(1, 0);

                    Rectangle front;
                    front.setPosition(0, x + chunkOffset.x,        y + 1.0f + chunkOffset.y, z + chunkOffset.z);
                    front.setPosition(1, x + chunkOffset.x,        y + chunkOffset.y,        z + chunkOffset.z);
                    front.setPosition(2, x + 1.0f + chunkOffset.x, y + chunkOffset.y,        z + chunkOffset.z);
                    front.setPosition(3, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y, z + chunkOffset.z);
                    front.setUV(0, 0.125f + (float)textureLocation.x * 0.125, 0.0f + (float)textureLocation.y * 0.125f);
                    front.setUV(1, 0.125f + (float)textureLocation.x * 0.125, 0.125f + (float)textureLocation.y * 0.125f);
                    front.setUV(2, 0.0f + (float)textureLocation.x * 0.125,   0.125f + (float)textureLocation.y * 0.125f);
                    front.setUV(3, 0.0f + (float)textureLocation.x * 0.125,   0.0f + (float)textureLocation.y * 0.125f);
                    front.calculateNormal();
                    vbo.PushRectangle(front);
                    
                    Rectangle back;
                    back.setPosition(0, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    back.setPosition(1, x + 1.0f + chunkOffset.x, y + chunkOffset.y,        z + 1.0f + chunkOffset.z);
                    back.setPosition(2, x + chunkOffset.x,        y + chunkOffset.y,        z + 1.0f + chunkOffset.z);
                    back.setPosition(3, x + chunkOffset.x,        y + 1.0f + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    back.setUV(0, 0.125f + (float)textureLocation.x * 0.125, 0.0f + (float)textureLocation.y * 0.125f);
                    back.setUV(1, 0.125f + (float)textureLocation.x * 0.125, 0.125f + (float)textureLocation.y * 0.125f);
                    back.setUV(2, 0.0f + (float)textureLocation.x * 0.125,   0.125f + (float)textureLocation.y * 0.125f);
                    back.setUV(3, 0.0f + (float)textureLocation.x * 0.125,   0.0f + (float)textureLocation.y * 0.125f);
                    back.calculateNormal();
                    vbo.PushRectangle(back);

                    Rectangle left;
                    left.setPosition(0, x + chunkOffset.x, y + 1.0f + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    left.setPosition(1, x + chunkOffset.x, y + chunkOffset.y,        z + 1.0f + chunkOffset.z);
                    left.setPosition(2, x + chunkOffset.x, y + chunkOffset.y,        z + chunkOffset.z);
                    left.setPosition(3, x + chunkOffset.x, y + 1.0f + chunkOffset.y, z + chunkOffset.z);
                    left.setUV(0, 0.125f + (float)textureLocation.x * 0.125, 0.0f + (float)textureLocation.y * 0.125f);
                    left.setUV(1, 0.125f + (float)textureLocation.x * 0.125, 0.125f + (float)textureLocation.y * 0.125f);
                    left.setUV(2, 0.0f + (float)textureLocation.x * 0.125,   0.125f + (float)textureLocation.y * 0.125f);
                    left.setUV(3, 0.0f + (float)textureLocation.x * 0.125,   0.0f + (float)textureLocation.y * 0.125f);
                    left.calculateNormal();
                    vbo.PushRectangle(left);

                    Rectangle right;
                    right.setPosition(0, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y, z + chunkOffset.z);
                    right.setPosition(1, x + 1.0f + chunkOffset.x, y + chunkOffset.y,        z + chunkOffset.z);
                    right.setPosition(2, x + 1.0f + chunkOffset.x, y + chunkOffset.y,        z + 1.0f + chunkOffset.z);
                    right.setPosition(3, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    right.setUV(0, 0.125f + (float)textureLocation.x * 0.125, 0.0f + (float)textureLocation.y * 0.125f);
                    right.setUV(1, 0.125f + (float)textureLocation.x * 0.125, 0.125f + (float)textureLocation.y * 0.125f);
                    right.setUV(2, 0.0f + (float)textureLocation.x * 0.125,   0.125f + (float)textureLocation.y * 0.125f);
                    right.setUV(3, 0.0f + (float)textureLocation.x * 0.125,   0.0f + (float)textureLocation.y * 0.125f);
                    right.calculateNormal();
                    vbo.PushRectangle(right);

                    Rectangle top;
                    top.setPosition(0, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    top.setPosition(1, x + chunkOffset.x,        y + 1.0f + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    top.setPosition(2, x + chunkOffset.x,        y + 1.0f + chunkOffset.y, z + chunkOffset.z);
                    top.setPosition(3, x + 1.0f + chunkOffset.x, y + 1.0f + chunkOffset.y, z + chunkOffset.z);
                    top.setUV(0, 0.125f + (float)textureLocation.x * 0.125, 0.0f + (float)textureLocation.y * 0.125f);
                    top.setUV(1, 0.125f + (float)textureLocation.x * 0.125, 0.125f + (float)textureLocation.y * 0.125f);
                    top.setUV(2, 0.0f + (float)textureLocation.x * 0.125,   0.125f + (float)textureLocation.y * 0.125f);
                    top.setUV(3, 0.0f + (float)textureLocation.x * 0.125,   0.0f + (float)textureLocation.y * 0.125f);
                    top.calculateNormal();
                    vbo.PushRectangle(top);

                    Rectangle bottom;
                    bottom.setPosition(0, x + 1.0f + chunkOffset.x, y + chunkOffset.y, z + chunkOffset.z);
                    bottom.setPosition(1, x + chunkOffset.x,        y + chunkOffset.y, z + chunkOffset.z);
                    bottom.setPosition(2, x + chunkOffset.x,        y + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    bottom.setPosition(3, x + 1.0f + chunkOffset.x, y + chunkOffset.y, z + 1.0f + chunkOffset.z);
                    bottom.setUV(0, 0.125f + (float)textureLocation.x * 0.125, 0.0f + (float)textureLocation.y * 0.125f);
                    bottom.setUV(1, 0.125f + (float)textureLocation.x * 0.125, 0.125f + (float)textureLocation.y * 0.125f);
                    bottom.setUV(2, 0.0f + (float)textureLocation.x * 0.125,   0.125f + (float)textureLocation.y * 0.125f);
                    bottom.setUV(3, 0.0f + (float)textureLocation.x * 0.125,   0.0f + (float)textureLocation.y * 0.125f);
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
