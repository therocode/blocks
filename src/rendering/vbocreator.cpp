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

VBO VBOCreator::generateChunkVBO(Chunk* mainChunk, Chunk* topChunk, Chunk* bottomChunk, Chunk* frontChunk, Chunk* backChunk, Chunk* leftChunk, Chunk* rightChunk)
{
    high_resolution_clock::time_point start = high_resolution_clock::now();

    //VBO vbo;
    VBO nvbo;

    nvbo.getVertexDeclaration().addElement(VertexElement::ELEMENT_FLOAT3, 0, "vert");
    nvbo.getVertexDeclaration().addElement(VertexElement::ELEMENT_FLOAT3, 1, "color");
    nvbo.getVertexDeclaration().addElement(VertexElement::ELEMENT_FLOAT3, 2, "normal");
    nvbo.getVertexDeclaration().addElement(VertexElement::ELEMENT_FLOAT2, 3, "uv");
    nvbo.getVertexDeclaration().addElement(VertexElement::ELEMENT_FLOAT4, 4, "bounds");

    const ChunkCoordinate location = mainChunk->getLocation();

    glm::vec3 chunkOffset(location.x * (float)chunkWidth, location.y * (float)chunkWidth, location.z * (float)chunkWidth);

    const VoxelTypeData& voxelTypeData = mainChunk->getVoxelTypeData();
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
            else
            {
                if(bottomChunk)
                {
                    bottomIterator = &bottomChunk->getVoxelTypeData().mRleSegments[bottomChunk->getVoxelTypeData().mRleSegmentIndices[z + (chunkWidth - 1) * chunkWidth].mSegmentStart];
                }
            }
            if(y < chunkWidth - 1)
                topIterator = &voxelTypeData.mRleSegments[voxelTypeData.mRleSegmentIndices[zyIndex + chunkWidth].mSegmentStart];
            else
            {
                if(topChunk)
                {
                    topIterator = &topChunk->getVoxelTypeData().mRleSegments[topChunk->getVoxelTypeData().mRleSegmentIndices[z].mSegmentStart];
                }
            }
            if(z > 0)
                backIterator = &voxelTypeData.mRleSegments[voxelTypeData.mRleSegmentIndices[zyIndex - 1].mSegmentStart];
            else
            {
                if(backChunk)
                {
                    backIterator = &backChunk->getVoxelTypeData().mRleSegments[backChunk->getVoxelTypeData().mRleSegmentIndices[y * chunkWidth + (chunkWidth - 1)].mSegmentStart];
                }
            }
            if(z < chunkWidth - 1)
                frontIterator = &voxelTypeData.mRleSegments[voxelTypeData.mRleSegmentIndices[zyIndex + 1].mSegmentStart];
            else
            {
                if(frontChunk)
                {
                    frontIterator = &frontChunk->getVoxelTypeData().mRleSegments[frontChunk->getVoxelTypeData().mRleSegmentIndices[y * chunkWidth].mSegmentStart];
                }
            }

            walker.setIterators(centreIterator, topIterator, bottomIterator, frontIterator, backIterator, y, z);

            walker.setLeftType(1);
            walker.setRightType(1);

            if(leftChunk)
            {
                walker.setLeftType(leftChunk->getVoxelType(chunkWidth - 1, y, z));
            }
            if(rightChunk)
            {
                walker.setRightType(rightChunk->getVoxelType(0, y, z));
            }

            walker.walk();
        }
    }

    unsigned int verts = 0, indices = 0;
    SurfaceMerger merger;
    glm::uvec2 textureLocation;
    ChunkRect* rect;

    unsigned int quads = 0;
    //extract top quads
    std::vector<SurfaceQuad> topQuads = walker.getTopQuads();
    merger.doSecondMerge(topQuads);
    quads += topQuads.size();

    //extract bottom quads
    std::vector<SurfaceQuad> bottomQuads = walker.getBottomQuads();
    merger.doSecondMerge(bottomQuads);
    quads += bottomQuads.size();

    //extract front quads
    std::vector<SurfaceQuad> frontQuads = walker.getFrontQuads();
    merger.doSecondMerge(frontQuads);
    quads += frontQuads.size();

    //extract back quads
    std::vector<SurfaceQuad> backQuads = walker.getBackQuads();
    merger.doSecondMerge(backQuads);
    quads += backQuads.size();

    //extract left quads
    std::vector<SurfaceQuad> leftQuads = walker.getLeftQuads();
    merger.doFirstMerge(leftQuads);
    merger.doSecondMerge(leftQuads);
    quads += leftQuads.size();

    //extract right quads
    std::vector<SurfaceQuad> rightQuads = walker.getRightQuads();
    merger.doFirstMerge(rightQuads);
    merger.doSecondMerge(rightQuads);
    quads += rightQuads.size();

    nvbo.setMaxSize(quads * 4, quads * 6);
    nvbo.allocateBuffers();
    nvbo.reset();

    for(auto& quad : topQuads)
    {
        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mDepth + chunkOffset.y;
        float worldZ = quad.mY + chunkOffset.z;
        int resId = quad.mType;
        textureLocation.x = (resId - 1) % 8;
        textureLocation.y = (resId - 1) / 8;
		rect->pushIndicesIntoVBO(nvbo);
        rect = (ChunkRect*)nvbo.getNextVertexPtr(4);
        rect->reset();
        setChunkRectData(*rect, worldX, worldY, worldZ, TOP, textureLocation.x, textureLocation.y, quad.mWidth, quad.mHeight);
        rect->setBounds(rect->vs[0].uv[0], rect->vs[0].uv[1], rect->vs[2].uv[0], rect->vs[2].uv[1]);
        rect->calculateNormal();
		rect->setUV(0, 0, 0);
        rect->setUV(1, 0, quad.mHeight);
        rect->setUV(2, quad.mWidth, quad.mHeight);
        rect->setUV(3, quad.mWidth, 0);

		float y = worldY * 0.001f;
		y = 1.0f - y;
		y = glm::clamp(y, 0.f, 1.f);

		rect->setColor(y,y,y);

        verts +=4;indices+=6;
    }

    for(auto& quad : bottomQuads)
    {
        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mDepth + chunkOffset.y;
        float worldZ = quad.mY + chunkOffset.z;
		int resId = quad.mType;
		if(quad.mType == 1)resId = 2;
        textureLocation.x = (resId - 1) % 8;
        textureLocation.y = (resId - 1) / 8;
		rect->pushIndicesIntoVBO(nvbo);
        rect = (ChunkRect*)nvbo.getNextVertexPtr(4);
        rect->reset();

        setChunkRectData(*rect, worldX, worldY, worldZ, BOTTOM, textureLocation.x, textureLocation.y, quad.mWidth, quad.mHeight);
        rect->setBounds(rect->vs[0].uv[0], rect->vs[0].uv[1], rect->vs[2].uv[0], rect->vs[2].uv[1]);
        rect->calculateNormal();
		rect->setUV(0, 0, 0);
        rect->setUV(1, 0, quad.mHeight);
        rect->setUV(2, quad.mWidth, quad.mHeight);
        rect->setUV(3, quad.mWidth, 0);

        verts +=4;indices+=6;
    }

    for(auto& quad : frontQuads)
    {
        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mY + chunkOffset.y;
        float worldZ = quad.mDepth + chunkOffset.z;

		int resId = quad.mType;
		if(quad.mType == 1)resId = 3;
        textureLocation.x = (resId - 1) % 8;
        textureLocation.y = (resId - 1) / 8;

		rect->pushIndicesIntoVBO(nvbo);
        rect = (ChunkRect*)nvbo.getNextVertexPtr(4);
        rect->reset();
        setChunkRectData(*rect, worldX, worldY, worldZ, FRONT, textureLocation.x, textureLocation.y, quad.mWidth, quad.mHeight);
        rect->setBounds(rect->vs[0].uv[0], rect->vs[0].uv[1], rect->vs[2].uv[0], rect->vs[2].uv[1]);
        rect->calculateNormal();
		rect->setUV(0, 0, 0);
        rect->setUV(1, 0, quad.mHeight);
        rect->setUV(2, quad.mWidth, quad.mHeight);
        rect->setUV(3, quad.mWidth, 0);

        verts +=4;indices+=6;
    }

    for(auto& quad : backQuads)
    {
        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mY + chunkOffset.y;
        float worldZ = quad.mDepth + chunkOffset.z;
		int resId = quad.mType;
		if(quad.mType == 1)resId = 3;
        textureLocation.x = (resId - 1) % 8;
        textureLocation.y = (resId - 1) / 8;

		rect->pushIndicesIntoVBO(nvbo);
        rect = (ChunkRect*)nvbo.getNextVertexPtr(4);
        rect->reset();
        setChunkRectData(*rect, worldX, worldY, worldZ, BACK, textureLocation.x, textureLocation.y, quad.mWidth, quad.mHeight);
        rect->setBounds(rect->vs[0].uv[0], rect->vs[0].uv[1], rect->vs[2].uv[0], rect->vs[2].uv[1]);
        rect->calculateNormal();
		rect->setUV(0, 0, 0);
        rect->setUV(1, 0, quad.mHeight);
        rect->setUV(2, quad.mWidth, quad.mHeight);
        rect->setUV(3, quad.mWidth, 0);

        verts +=4;indices+=6;
    }

    for(auto& quad : leftQuads)
    {
        float worldX = quad.mDepth + chunkOffset.x;
        float worldY = quad.mY + chunkOffset.y;
        float worldZ = quad.mX + chunkOffset.z;

		int resId = quad.mType;
		if(quad.mType == 1)resId = 3;
        textureLocation.x = (resId - 1) % 8;
        textureLocation.y = (resId - 1) / 8;

		rect->pushIndicesIntoVBO(nvbo);
        rect = (ChunkRect*)nvbo.getNextVertexPtr(4);
        rect->reset();
        setChunkRectData(*rect, worldX, worldY, worldZ, LEFT, textureLocation.x, textureLocation.y, quad.mWidth, quad.mHeight);
        rect->setBounds(rect->vs[0].uv[0], rect->vs[0].uv[1], rect->vs[2].uv[0], rect->vs[2].uv[1]);
        rect->calculateNormal();
		rect->setUV(0, 0, 0);
        rect->setUV(1, 0, quad.mHeight);
        rect->setUV(2, quad.mWidth, quad.mHeight);
        rect->setUV(3, quad.mWidth, 0);

        verts +=4;indices+=6;
    }

    for(auto& quad : rightQuads)
    {
        float worldX = quad.mDepth + chunkOffset.x - 1.0f;
        float worldY = quad.mY + chunkOffset.y;
        float worldZ = quad.mX + chunkOffset.z;

		int resId = quad.mType;
		if(quad.mType == 1)resId = 3;
        textureLocation.x = (resId - 1) % 8;
        textureLocation.y = (resId - 1) / 8;
        
		rect->pushIndicesIntoVBO(nvbo);
        rect = (ChunkRect*)nvbo.getNextVertexPtr(4);
        rect->reset();
        setChunkRectData(*rect, worldX, worldY, worldZ, RIGHT, textureLocation.x, textureLocation.y, quad.mWidth, quad.mHeight);
        rect->setBounds(rect->vs[0].uv[0], rect->vs[0].uv[1], rect->vs[2].uv[0], rect->vs[2].uv[1]);
        rect->calculateNormal();
		rect->setUV(0, 0, 0);
        rect->setUV(1, 0, quad.mHeight);
        rect->setUV(2, quad.mWidth, quad.mHeight);
        rect->setUV(3, quad.mWidth, 0);

        verts +=4;indices+=6;
    }

    //printf("Generated %u vertices and %u indices\n", verts, indices);
    //After stuff has been added, you have to update the gpu vbo data.
    nvbo.uploadVBO();
    nvbo.clear();

    //totalAmount += amount / 2;
    high_resolution_clock::time_point end = high_resolution_clock::now();
    totalTime += duration_cast<microseconds>(end - start).count();
    timesGenerated++;
    //std::cout << "the mesh creation of the chunk took " << duration_cast<microseconds>(end - start).count() << " and the average is " << totalTime/ timesGenerated << ". it had " << amount << " faces and the average is " << totalAmount / timesGenerated<< "\n";

    return nvbo;
}
/*
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

}*/
inline void VBOCreator::setChunkRectData(ChunkRect& r, float x, float y, float z, int face, float u, float v, float width, float height) const
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
	vbo.getVertexDeclaration().addElement(VertexElement::ELEMENT_FLOAT3, 0, "vert");
    vbo.getVertexDeclaration().addElement(VertexElement::ELEMENT_FLOAT3, 1, "color");
    vbo.getVertexDeclaration().addElement(VertexElement::ELEMENT_FLOAT3, 2, "normal");
    vbo.getVertexDeclaration().addElement(VertexElement::ELEMENT_FLOAT2, 3, "uv");
	vbo.setMaxSize(4, 6);
	vbo.allocateBuffers();

	glm::uvec2 textureLocation(0, 2);
	BillboardRect* billRect;
	billRect->pushIndicesIntoVBO(vbo);
	billRect = (BillboardRect*)vbo.getNextVertexPtr(4);
	if(billRect != NULL){
		billRect->reset();
		// setChunkRectData(*(ChunkRect*)billRect, 0.0f, 0.0f, 0.0f, CENTER, (float)textureLocation.x, (float)textureLocation.y, 1.0f, 1.0f);
		billRect->setColor(1.f, 1.f, 1.f);
		float scaleX = 0.5f;
		float scaleY = 0.8f;
		billRect->setPosition(0, -1 * scaleX, 1 * scaleY, 0);
		billRect->setPosition(1, -1 * scaleX,-1 * scaleY, 0);
		billRect->setPosition(2,  1 * scaleX,-1 * scaleY, 0);
		billRect->setPosition(3,  1 * scaleX, 1 * scaleY, 0);

		float u = textureLocation.x;
		float v = textureLocation.y;
		float uo, vo;
		uo = vo	= 0.125f;

		#ifdef EMSCRIPTEN
			float e = 0.006f;
		#else
			float e = 0.0006f;
		#endif

		billRect->setUV(0, e+      (float)u * uo,  e+      (float)v * vo);
		billRect->setUV(1, e+      (float)u * uo, -e+ vo + (float)v * vo);
		billRect->setUV(2,-e+ uo + (float)u * uo, -e+ vo + (float)v * vo);
		billRect->setUV(3,-e+ uo + (float)u * uo,  e+      (float)v * vo);

		billRect->calculateNormal();
		// printf("Generated billboard.\n");
	}
	vbo.uploadVBO();
	vbo.clear();

    return vbo;
}
