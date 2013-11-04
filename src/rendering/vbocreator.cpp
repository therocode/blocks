#include "vbocreator.h"
#include <vector>
#include <chrono>
#include "meshwalker.h"

using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::high_resolution_clock;

uint32_t VBOCreator::totalTime = 0;
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

            walker.setIterators(centreIterator, topIterator, bottomIterator, frontIterator, backIterator, y);
            walker.walk();
        }
    }

    SurfaceMerger merger;
    std::cout << "walker giving " << walker.getTopQuads().size() << " quads to merger\n";
    merger.setQuads(walker.getTopQuads());

    merger.doSecondMerge();

    
	glm::uvec2 textureLocation;
    Rectangle r;

    for(auto& quad : merger.getQuads())
    {
        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mDepth + chunkOffset.y - 1.0f;
        float worldZ = quad.mY + chunkOffset.z;

        textureLocation = glm::uvec2(quad.mType - 1, 0);
        r.setPosition(3, worldX,               worldY, worldZ + quad.mHeight);
        r.setPosition(2, worldX,               worldY, worldZ               );
        r.setPosition(1, worldX + quad.mWidth, worldY, worldZ               );
        r.setPosition(0, worldX + quad.mWidth, worldY, worldZ + quad.mHeight);

        //std::cout << "adding rectangle: " << quad.mX << " " << quad.mY << " " << quad.mWidth << " " << quad.mHeight << " depth: " << quad.mDepth << " type: " << quad.mType << "\n";

        float u = textureLocation.x;
        float v = textureLocation.y;

        float uo = 0.125f;
        float vo = 0.125f;
#ifdef EMSCRIPTEN
        float e = 0.006f;
#else
        float e = 0.0006f;
#endif
        r.setUV(3, e+      (float)u * uo,  e+      (float)v * vo);
        r.setUV(2, e+      (float)u * uo, -e+ vo + (float)v * vo);
        r.setUV(1,(-e+ uo + (float)u * uo) * (float)quad.mWidth, -e+ vo + (float)v * vo);
        r.setUV(0,(-e+ uo + (float)u * uo) * (float)quad.mWidth,  e+      (float)v * vo);

        r.calculateNormal();
        vbo.PushRectangle(r);
    }

	//After stuff has been added, you have to update the gpu vbo data.
	vbo.UpdateVBO();
	vbo.Clear();
    

    high_resolution_clock::time_point end = high_resolution_clock::now();
    totalTime += duration_cast<microseconds>(end - start).count();
    timesGenerated++;
    //std::cout << "the mesh creation of the chunk took " << duration_cast<microseconds>(end - start).count() << " and the average is " << totalTime/ timesGenerated << "\n";

	return vbo;
}

inline void VBOCreator::setRectData(Rectangle& r, float x, float y, float z, int face, float u, float v) const
{
	float boxSize = 1.f;
	float hs      = boxSize;
	float nhs = 0.f;
	switch(face){
		case FRONT:
			z += hs;
			r.setPosition(0, x + nhs,  y + hs, z);
			r.setPosition(1, x + nhs,  y + nhs, z);
			r.setPosition(2, x + hs,  y + nhs, z);
			r.setPosition(3, x + hs,  y + hs, z);
			break;
		case RIGHT:
			x += hs;
			r.setPosition(0, x, y + hs, z + hs);
			r.setPosition(1, x, y + nhs, z + hs);
			r.setPosition(2, x, y + nhs, z + nhs);
			r.setPosition(3, x, y + hs, z + nhs);
			break;
		case BACK:
			//z -= hs;
			r.setPosition(0, x + hs,  y + hs, z);
			r.setPosition(1, x + hs,  y + nhs, z);
			r.setPosition(2, x + nhs,  y + nhs, z);
			r.setPosition(3, x + nhs,  y + hs, z);
			break;
		case LEFT:
			//x -= hs;
			r.setPosition(0, x, y + hs, z + nhs);
			r.setPosition(1, x, y + nhs, z + nhs);
			r.setPosition(2, x, y + nhs, z + hs);
			r.setPosition(3, x, y + hs, z + hs);
			break;
		case TOP:
			y += hs;
			r.setPosition(0, x + nhs, y, z + nhs);
			r.setPosition(1, x + nhs, y, z + hs);
			r.setPosition(2, x + hs, y, z + hs);
			r.setPosition(3, x + hs, y, z + nhs);
			break;
		case BOTTOM:
			//y -= hs;
			r.setPosition(0, x + hs, y, z + nhs);
			r.setPosition(1, x + hs, y, z + hs);
			r.setPosition(2, x + nhs, y, z + hs);
			r.setPosition(3, x + nhs, y, z + nhs);
			break;
		case CENTER:
			hs *= 0.5f;
			nhs = -hs;
			r.setPosition(0, x + nhs,  y + hs, z);
			r.setPosition(1, x + nhs,  y + nhs, z);
			r.setPosition(2, x + hs,  y + nhs, z);
			r.setPosition(3, x + hs,  y + hs, z);
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
    setRectData(r, 0.0f, 0.0f, 0.0f, CENTER, (float)textureLocation.x, (float)textureLocation.y);
    r.calculateNormal();
    vbo.PushRectangle(r);

	vbo.UpdateVBO();

    return vbo;
}
