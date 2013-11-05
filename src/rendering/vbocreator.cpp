#include "vbocreator.h"
#include <vector>
#include <chrono>
#include "meshwalker.h"

using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::high_resolution_clock;

uint32_t VBOCreator::totalTime = 0;
uint32_t VBOCreator::totalAmount = 0;
uint32_t VBOCreator::timesGenerated = 0;

VBOCreator::VBOCreator()
{
}

VBO VBOCreator::generateChunkVBO(const ChunkCoordinate& coord, const VoxelTypeData& voxelTypeData)
{
    high_resolution_clock::time_point start = high_resolution_clock::now();
//int texture1X = rand() % 8;
//int texture2X = rand() % 8;
//int texture1Y = rand() % 2;
//int texture2Y = rand() % 2;

	VBO vbo;

	const ChunkCoordinate location = coord;

	glm::vec3 chunkOffset(location.x * (float)chunkWidth, location.y * (float)chunkWidth, location.z * (float)chunkWidth);

	VoxelTypeArray voxelTypes;
    MeshWalker walker;

    for(uint32_t z = 0; z < chunkWidth; z++)
    {
        for(uint32_t y = 0; y < chunkWidth; y++)
        {
            size_t zyIndex = z + y * chunkWidth;

            RleIterator centreIterator = &voxelTypeData.mRleSegments[voxelTypeData.mRleSegmentIndices[zyIndex].mSegmentStart];
            RleIterator topIterator = nullptr;
            RleIterator bottomIterator = nullptr;
            RleIterator frontIterator = nullptr;
            RleIterator backIterator = nullptr;

            if(y > 0)
                bottomIterator = &voxelTypeData.mRleSegments[voxelTypeData.mRleSegmentIndices[zyIndex - chunkWidth].mSegmentStart];
            if(y < chunkWidth - 1)
                topIterator = &voxelTypeData.mRleSegments[voxelTypeData.mRleSegmentIndices[zyIndex + chunkWidth].mSegmentStart];
            if(z > 0)
                backIterator = &voxelTypeData.mRleSegments[voxelTypeData.mRleSegmentIndices[zyIndex - 1].mSegmentStart];
            if(z < chunkWidth - 1)
                frontIterator = &voxelTypeData.mRleSegments[voxelTypeData.mRleSegmentIndices[zyIndex + 1].mSegmentStart];

            walker.setIterators(centreIterator, topIterator, bottomIterator, frontIterator, backIterator, y, z);
            walker.walk();
        }
    }

    SurfaceMerger merger;
	glm::uvec2 textureLocation;
    Rectangle r;

    //extract top quads
    merger.setQuads(walker.getTopQuads());
    merger.doSecondMerge();
    for(auto& quad : merger.getQuads())
    {
        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mDepth + chunkOffset.y;
        float worldZ = quad.mY + chunkOffset.z;

        textureLocation = glm::uvec2(quad.mType - 1, 0);
        setRectData(r, worldX, worldY, worldZ, TOP, textureLocation.x, textureLocation.y, quad.mWidth, quad.mHeight);

        r.calculateNormal();
        vbo.PushRectangle(r);
    }

    //extract bottom quads
    merger.setQuads(walker.getBottomQuads());
    merger.doSecondMerge();
    for(auto& quad : merger.getQuads())
    {
        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mDepth + chunkOffset.y;
        float worldZ = quad.mY + chunkOffset.z;

        textureLocation = glm::uvec2(quad.mType - 1, 0);
        setRectData(r, worldX, worldY, worldZ, BOTTOM, textureLocation.x, textureLocation.y, quad.mWidth, quad.mHeight);

        r.calculateNormal();
        vbo.PushRectangle(r);
    }

    //extract front quads
    merger.setQuads(walker.getFrontQuads());
    merger.doSecondMerge();
    for(auto& quad : merger.getQuads())
    {
        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mY + chunkOffset.y;
        float worldZ = quad.mDepth + chunkOffset.z;

        textureLocation = glm::uvec2(quad.mType - 1, 0);
        setRectData(r, worldX, worldY, worldZ, FRONT, textureLocation.x, textureLocation.y, quad.mWidth, quad.mHeight);

        r.calculateNormal();
        vbo.PushRectangle(r);
    }

    //extract back quads
    merger.setQuads(walker.getBackQuads());
    merger.doSecondMerge();
    for(auto& quad : merger.getQuads())
    {
        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mY + chunkOffset.y;
        float worldZ = quad.mDepth + chunkOffset.z;

        textureLocation = glm::uvec2(quad.mType - 1, 0);
        setRectData(r, worldX, worldY, worldZ, BACK, textureLocation.x, textureLocation.y, quad.mWidth, quad.mHeight);

        r.calculateNormal();
        vbo.PushRectangle(r);
    }

    //extract left quads
    merger.setQuads(walker.getLeftQuads());
    merger.doFirstMerge();
    merger.doSecondMerge();
    for(auto& quad : merger.getQuads())
    {
        float worldX = quad.mDepth + chunkOffset.x;
        float worldY = quad.mY + chunkOffset.y;
        float worldZ = quad.mX + chunkOffset.z;

        textureLocation = glm::uvec2(quad.mType - 1, 0);
        setRectData(r, worldX, worldY, worldZ, LEFT, textureLocation.x, textureLocation.y, quad.mWidth, quad.mHeight);

        r.calculateNormal();
        vbo.PushRectangle(r);
    }

    //extract right quads
    merger.setQuads(walker.getRightQuads());
    merger.doFirstMerge();
    merger.doSecondMerge();
    for(auto& quad : merger.getQuads())
    {
        float worldX = quad.mDepth + chunkOffset.x - 1.0f;
        float worldY = quad.mY + chunkOffset.y;
        float worldZ = quad.mX + chunkOffset.z;

        textureLocation = glm::uvec2(quad.mType - 1, 0);
        setRectData(r, worldX, worldY, worldZ, RIGHT, textureLocation.x, textureLocation.y, quad.mWidth, quad.mHeight);

        r.calculateNormal();
        vbo.PushRectangle(r);
    }

	//After stuff has been added, you have to update the gpu vbo data.
	vbo.UpdateVBO();
    uint32_t amount = vbo.GetDrawAmount();
	vbo.Clear();
    

    totalAmount += amount / 2;
    high_resolution_clock::time_point end = high_resolution_clock::now();
    totalTime += duration_cast<microseconds>(end - start).count();
    timesGenerated++;
    //std::cout << "the mesh creation of the chunk took " << duration_cast<microseconds>(end - start).count() << " and the average is " << totalTime/ timesGenerated << ". it had " << amount << " faces and the average is " << totalAmount / timesGenerated<< "\n";

	return vbo;
}

inline void VBOCreator::setRectData(Rectangle& r, float x, float y, float z, int face, float u, float v, float width, float height) const
{
	float nhs = 0.f;
	switch(face){
		case FRONT:
			z += 1.0f;
			r.setPosition(0, x + nhs,  y + height, z);
			r.setPosition(1, x + nhs,  y + nhs, z);
			r.setPosition(2, x + width,  y + nhs, z);
			r.setPosition(3, x + width,  y + height, z);
			break;
		case RIGHT:
			x += 1.0f;
			r.setPosition(0, x, y + height, z + width);
			r.setPosition(1, x, y + nhs, z + width);
			r.setPosition(2, x, y + nhs, z + nhs);
			r.setPosition(3, x, y + height, z + nhs);
			break;
		case BACK:
			//z -= hs;
			r.setPosition(0, x + width,  y + height, z);
			r.setPosition(1, x + width,  y + nhs, z);
			r.setPosition(2, x + nhs,  y + nhs, z);
			r.setPosition(3, x + nhs,  y + height, z);
			break;
		case LEFT:
			//x -= hs;
			r.setPosition(0, x, y + height, z + nhs);
			r.setPosition(1, x, y + nhs, z + nhs);
			r.setPosition(2, x, y + nhs, z + width);
			r.setPosition(3, x, y + height, z + width);
			break;
		case TOP:
			y += 1.0f;
			r.setPosition(0, x + nhs, y, z + nhs);
			r.setPosition(1, x + nhs, y, z + height);
			r.setPosition(2, x + width, y, z + height);
			r.setPosition(3, x + width, y, z + nhs);
			break;
		case BOTTOM:
			//y -= hs;
			r.setPosition(0, x + width, y, z + nhs);
			r.setPosition(1, x + width, y, z + height);
			r.setPosition(2, x + nhs, y, z + height);
			r.setPosition(3, x + nhs, y, z + nhs);
			break;
		case CENTER:
            width *= 0.5f;
            height *= 0.5f;
			nhs = -width;
			r.setPosition(0, x + nhs,  y + height, z);
			r.setPosition(1, x + nhs,  y + nhs, z);
			r.setPosition(2, x + width,  y + nhs, z);
			r.setPosition(3, x + width,  y + height, z);
			break;
	}

	float uo, vo;
	uo = vo	= 0.125f;
#ifdef EMSCRIPTEN
	float e = 0.006f;
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
    setRectData(r, 0.0f, 0.0f, 0.0f, CENTER, (float)textureLocation.x, (float)textureLocation.y, 1.0f, 1.0f);
    r.calculateNormal();
    vbo.PushRectangle(r);

	vbo.UpdateVBO();

    return vbo;
}
