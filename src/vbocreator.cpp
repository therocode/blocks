#include "vbocreator.h"
#include <vector>
#include <iostream>

VBO VBOCreator::generateChunkVBO(const Chunk& chunk) const
{
	VBO vbo;

	const ChunkCoordinate location = chunk.getLocation();

	glm::vec3 chunkOffset(location.x * (float)chunkWidth, location.y * (float)chunkWidth, location.z * (float)chunkWidth);

	const VoxelTypeArray& voxelTypes = chunk.getVoxelTypes();

	glm::uvec2 textureLocation;

	std::cout << "generating a vbo for a chunk with location: " << chunkOffset.x << " " << chunkOffset.y << " " << chunkOffset.z << "\n";

	uint32_t xStep = 1;
	uint32_t yStep = chunkWidth;
	uint32_t zStep = chunkWidthx2;
	uint32_t currentIndex = 0;

	for(int z = 0; z < chunkWidth; z++)
	{
		for(int y = 0; y < chunkWidth; y++)
		{
			for(int x = 0; x < chunkWidth; x++)
			{
				currentIndex = x + y * chunkWidth + z * chunkWidthx2;
				VoxelType type = voxelTypes[currentIndex];
				if(type != 0)
				{

					if(type == 1)
						textureLocation = glm::uvec2(0, 0);
					if(type == 2)
						textureLocation = glm::uvec2(1, 0);

					bool frontObscured = false;
					bool backObscured = false;
					bool leftObscured = false;
					bool rightObscured = false;
					bool topObscured = false;
					bool bottomObscured = false;

					if(x > 0)
					{
						if(voxelTypes[currentIndex - xStep] != 0)
							leftObscured = true;
					}
					if(x < chunkWidth - 1)
					{
						if(voxelTypes[currentIndex + xStep] != 0)
							rightObscured = true;
					}
					if(y > 0)
					{
						if(voxelTypes[currentIndex - yStep] != 0)
							bottomObscured = true;
					}
					if(y < chunkWidth - 1)
					{
						if(voxelTypes[currentIndex + yStep] != 0)
							topObscured = true;
					}
					if(z > 0)
					{
						if(voxelTypes[currentIndex - zStep] != 0)
							backObscured = true;
					}
					if(z < chunkWidth - 1)
					{
						if(voxelTypes[currentIndex + zStep] != 0)
							frontObscured = true;
					}

					float worldX = x + chunkOffset.x;
					float worldY = y + chunkOffset.y;
					float worldZ = z + chunkOffset.z;

					Rectangle r;
					if(!frontObscured)
					{
						setRectData(r, worldX, worldY, worldZ, FRONT, (float)textureLocation.x, (float)textureLocation.y);
						r.calculateNormal();
						vbo.PushRectangle(r);
					}

					if(!backObscured)
					{
						setRectData(r, worldX, worldY, worldZ, BACK, (float)textureLocation.x, (float)textureLocation.y);
						r.calculateNormal();
						vbo.PushRectangle(r);
					}

					if(!leftObscured)
					{
						setRectData(r, worldX, worldY, worldZ, LEFT, (float)textureLocation.x, (float)textureLocation.y);
						r.calculateNormal();
						vbo.PushRectangle(r);
					}

					if(!rightObscured)
					{
						setRectData(r, worldX, worldY, worldZ, RIGHT, (float)textureLocation.x, (float)textureLocation.y);
						r.calculateNormal();
						vbo.PushRectangle(r);
					}

					if(!topObscured)
					{
						setRectData(r, worldX, worldY, worldZ, TOP, (float)textureLocation.x, (float)textureLocation.y);
						r.calculateNormal();
						vbo.PushRectangle(r);
					}

					if(!bottomObscured)
					{
						setRectData(r, worldX, worldY, worldZ, BOTTOM, (float)textureLocation.x, (float)textureLocation.y);
						r.calculateNormal();
						vbo.PushRectangle(r);
					}
				}
			}
		}
	}

	//After stuff has been added, you have to update the gpu vbo data.
	vbo.UpdateVBO();

	return vbo;
}

inline void VBOCreator::setRectData(Rectangle& r, float x, float y, float z, int face, float u, float v) const
{
	float boxSize = 1.f;
	float hs      = boxSize * 0.5f;
	switch(face){
		case FRONT:
			z += hs;
			r.setPosition(0, x - hs,  y + hs, z);
			r.setPosition(1, x - hs,  y - hs, z);
			r.setPosition(2, x + hs,  y - hs, z);
			r.setPosition(3, x + hs,  y + hs, z);
			break;
		case RIGHT:
			x += hs;
			r.setPosition(0, x, y + hs, z + hs);
			r.setPosition(1, x, y - hs, z + hs);
			r.setPosition(2, x, y - hs, z - hs);
			r.setPosition(3, x, y + hs, z - hs);
			break;
		case BACK:
			z -= hs;
			r.setPosition(0, x + hs,  y + hs, z);
			r.setPosition(1, x + hs,  y - hs, z);
			r.setPosition(2, x - hs,  y - hs, z);
			r.setPosition(3, x - hs,  y + hs, z);
			break;
		case LEFT:
			x -= hs;
			r.setPosition(0, x, y + hs, z - hs);
			r.setPosition(1, x, y - hs, z - hs);
			r.setPosition(2, x, y - hs, z + hs);
			r.setPosition(3, x, y + hs, z + hs);
			break;
		case TOP:
			y += hs;
			r.setPosition(0, x - hs, y, z - hs);
			r.setPosition(1, x - hs, y, z + hs);
			r.setPosition(2, x + hs, y, z + hs);
			r.setPosition(3, x + hs, y, z - hs);
			break;
		case BOTTOM:
			y -= hs;
			r.setPosition(0, x + hs, y, z - hs);
			r.setPosition(1, x + hs, y, z + hs);
			r.setPosition(2, x - hs, y, z + hs);
			r.setPosition(3, x - hs, y, z - hs);
			break;
		case CENTER:
			r.setPosition(0, x - hs,  y + hs, z);
			r.setPosition(1, x - hs,  y - hs, z);
			r.setPosition(2, x + hs,  y - hs, z);
			r.setPosition(3, x + hs,  y + hs, z);
			break;
	}

	float uo, vo;
	uo = vo	= 0.125f;
#ifdef EMSCRIPTEN
	float e = 0.001f;
#else
	float e = 0.0006f;
#endif
	r.setUV(0, e+      (float)u * uo,  e+      (float)v * vo);
	r.setUV(1, e+      (float)u * uo, -e+ vo + (float)v * vo);
	r.setUV(2,-e+ uo + (float)u * uo, -e+ vo + (float)v * vo);
	r.setUV(3,-e+ uo + (float)u * uo,  e+      (float)v * vo);

}

VBO VBOCreator::generateBoardVBO(const glm::vec2& dimensions) const
{
    VBO vbo;
    glm::uvec2 textureLocation(0, 2);

    Rectangle r;
    setRectData(r, 0.0f, 0.0f, 0.0f, CENTER, (float)textureLocation.x, (float)textureLocation.y);
    r.calculateNormal();
    vbo.PushRectangle(r);

	vbo.UpdateVBO();

    return vbo;
}
