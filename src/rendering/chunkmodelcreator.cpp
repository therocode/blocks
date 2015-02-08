#include "chunkmodelcreator.hpp"
#include <vector>
#include <iostream>
#include "meshwalker.hpp"
#include "../world/chunk.hpp"
#include "modelattribute.hpp"

ChunkModel ChunkModelCreator::generateChunkModel(const ChunkCoord& mainChunkCoord, Chunk* mainChunk, Chunk* topChunk, Chunk* bottomChunk, Chunk* frontChunk, Chunk* backChunk, Chunk* leftChunk, Chunk* rightChunk)
{
    ChunkModel newModel;

    glm::vec3 chunkOffset(mainChunkCoord.x * (float)chunkWidth, mainChunkCoord.y * (float)chunkWidth, mainChunkCoord.z * (float)chunkWidth);

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
                walker.setLeftType(leftChunk->getVoxelType({chunkWidth - 1u, y, z}));
            }
            if(rightChunk)
            {
                walker.setRightType(rightChunk->getVoxelType({0u, y, z}));
            }

            walker.walk();
        }
    }

    unsigned int verts = 0, indices = 0;
    SurfaceMerger merger;
    glm::uvec2 textureLocation;

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
    
    std::vector<float> quadPositions;
    std::vector<float> quadNormals;
    std::vector<float> quadTexCoords;
    std::vector<float> allQuadPositions;
    std::vector<float> allQuadNormals;
    std::vector<float> allQuadTexCoords;
    std::vector<uint32_t> allTextureIndices;

    for(auto& quad : topQuads)
    {
        quadPositions.clear();
        quadNormals.clear();
        quadTexCoords.clear();

        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mDepth + chunkOffset.y + 1.0f; //top alignmetn
        float worldZ = quad.mY + chunkOffset.z;

        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);

        quadPositions = { worldX              , worldY, worldZ,
                          worldX              , worldY, worldZ + quad.mHeight,
                          worldX + quad.mWidth, worldY, worldZ + quad.mHeight,
                            
                          worldX              , worldY, worldZ,
                          worldX + quad.mWidth, worldY, worldZ + quad.mHeight,
                          worldX + quad.mWidth, worldY, worldZ };

        quadNormals   = { 0.0f,  1.0f, 0.0f,
                          0.0f,  1.0f, 0.0f,
                          0.0f,  1.0f, 0.0f,
        
                          0.0f,  1.0f, 0.0f,
                          0.0f,  1.0f, 0.0f,
                          0.0f,  1.0f, 0.0f};

        glm::vec2 texLoc;
        glm::vec2 plus(1.0f * quad.mWidth, 1.0f * quad.mHeight);

        quadTexCoords = {  texLoc.x         ,  texLoc.y         ,
                           texLoc.x         ,  texLoc.y + plus.y,
                           texLoc.x + plus.x,  texLoc.y + plus.y,
        
                           texLoc.x         ,  texLoc.y         ,
                           texLoc.x + plus.x,  texLoc.y + plus.y,
                           texLoc.x + plus.x,  texLoc.y};

        allQuadPositions.insert(allQuadPositions.end(), quadPositions.begin(), quadPositions.end());
        allQuadNormals.insert(allQuadNormals.end(), quadNormals.begin(), quadNormals.end());
        allQuadTexCoords.insert(allQuadTexCoords.end(), quadTexCoords.begin(), quadTexCoords.end());
    }

    for(auto& quad : bottomQuads)
    {
        quadPositions.clear();
        quadNormals.clear();
        quadTexCoords.clear();

        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mDepth + chunkOffset.y;
        float worldZ = quad.mY + chunkOffset.z;

        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);

        quadPositions = { worldX              , worldY, worldZ,
                          worldX + quad.mWidth, worldY, worldZ + quad.mHeight,
                          worldX              , worldY, worldZ + quad.mHeight,
                            
                          worldX              , worldY, worldZ,
                          worldX + quad.mWidth, worldY, worldZ,
                          worldX + quad.mWidth, worldY, worldZ + quad.mHeight};

        quadNormals   = { 0.0f, -1.0f, 0.0f,
                          0.0f, -1.0f, 0.0f,
                          0.0f, -1.0f, 0.0f,
        
                          0.0f, -1.0f, 0.0f,
                          0.0f, -1.0f, 0.0f,
                          0.0f, -1.0f, 0.0f};

        glm::vec2 texLoc;
        glm::vec2 plus(1.0f * quad.mWidth, 1.0f * quad.mHeight);

        quadTexCoords = {  texLoc.x         ,  texLoc.y         ,
                           texLoc.x + plus.x,  texLoc.y + plus.y,
                           texLoc.x         ,  texLoc.y + plus.y,
        
                           texLoc.x         ,  texLoc.y         ,
                           texLoc.x + plus.x,  texLoc.y         ,
                           texLoc.x + plus.x,  texLoc.y + plus.y};


        allQuadPositions.insert(allQuadPositions.end(), quadPositions.begin(), quadPositions.end());
        allQuadNormals.insert(allQuadNormals.end(), quadNormals.begin(), quadNormals.end());
        allQuadTexCoords.insert(allQuadTexCoords.end(), quadTexCoords.begin(), quadTexCoords.end());
    }

    for(auto& quad : frontQuads)
    {
        quadPositions.clear();
        quadNormals.clear();
        quadTexCoords.clear();

        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mY + chunkOffset.y;
        float worldZ = quad.mDepth + chunkOffset.z + 1.0f;

        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);

        quadPositions = { worldX              , worldY,                worldZ,
                          worldX + quad.mWidth, worldY + quad.mHeight, worldZ,
                          worldX              , worldY + quad.mHeight, worldZ,
                            
                          worldX              , worldY,                worldZ,
                          worldX + quad.mWidth, worldY,                worldZ,
                          worldX + quad.mWidth, worldY + quad.mHeight, worldZ};

        quadNormals   = { 0.0f, 0.0f,  1.0f,
                          0.0f, 0.0f,  1.0f,
                          0.0f, 0.0f,  1.0f,
        
                          0.0f, 0.0f,  1.0f,
                          0.0f, 0.0f,  1.0f,
                          0.0f, 0.0f,  1.0f};

        glm::vec2 texLoc;
        glm::vec2 plus(1.0f * quad.mWidth, 1.0f * quad.mHeight);

        quadTexCoords = {  texLoc.x         ,  texLoc.y         ,
                           texLoc.x + plus.x,  texLoc.y + plus.y,
                           texLoc.x         ,  texLoc.y + plus.y,
        
                           texLoc.x         ,  texLoc.y         ,
                           texLoc.x + plus.x,  texLoc.y         ,
                           texLoc.x + plus.x,  texLoc.y + plus.y};

        allQuadPositions.insert(allQuadPositions.end(), quadPositions.begin(), quadPositions.end());
        allQuadNormals.insert(allQuadNormals.end(), quadNormals.begin(), quadNormals.end());
        allQuadTexCoords.insert(allQuadTexCoords.end(), quadTexCoords.begin(), quadTexCoords.end());
    }

    for(auto& quad : backQuads)
    {
        quadPositions.clear();
        quadNormals.clear();
        quadTexCoords.clear();

        float worldX = quad.mX + chunkOffset.x;
        float worldY = quad.mY + chunkOffset.y;
        float worldZ = quad.mDepth + chunkOffset.z;

        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);

        quadPositions = { worldX              , worldY,                worldZ,
                          worldX              , worldY + quad.mHeight, worldZ,
                          worldX + quad.mWidth, worldY + quad.mHeight, worldZ,
                            
                          worldX              , worldY,                worldZ,
                          worldX + quad.mWidth, worldY + quad.mHeight, worldZ,
                          worldX + quad.mWidth, worldY,                worldZ};

        quadNormals   = { 0.0f, 0.0f,  -1.0f,
                          0.0f, 0.0f,  -1.0f,
                          0.0f, 0.0f,  -1.0f,
        
                          0.0f, 0.0f,  -1.0f,
                          0.0f, 0.0f,  -1.0f,
                          0.0f, 0.0f,  -1.0f};

        glm::vec2 texLoc;
        glm::vec2 plus(1.0f * quad.mWidth, 1.0f * quad.mHeight);

        quadTexCoords = {  texLoc.x         ,  texLoc.y         ,
                           texLoc.x         ,  texLoc.y + plus.y,
                           texLoc.x + plus.x,  texLoc.y + plus.y,
        
                           texLoc.x         ,  texLoc.y         ,
                           texLoc.x + plus.x,  texLoc.y + plus.y,
                           texLoc.x + plus.x,  texLoc.y         };

        allQuadPositions.insert(allQuadPositions.end(), quadPositions.begin(), quadPositions.end());
        allQuadNormals.insert(allQuadNormals.end(), quadNormals.begin(), quadNormals.end());
        allQuadTexCoords.insert(allQuadTexCoords.end(), quadTexCoords.begin(), quadTexCoords.end());
    }

    for(auto& quad : leftQuads)
    {
        quadPositions.clear();
        quadNormals.clear();
        quadTexCoords.clear();

        float worldX = quad.mDepth + chunkOffset.x;
        float worldY = quad.mY + chunkOffset.y;
        float worldZ = quad.mX + chunkOffset.z;

        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);

        quadPositions = { worldX              , worldY,                worldZ,
                          worldX              , worldY + quad.mHeight, worldZ + quad.mWidth,
                          worldX              , worldY + quad.mHeight, worldZ,
                            
                          worldX              , worldY,                worldZ,
                          worldX              , worldY,                worldZ + quad.mWidth,
                          worldX              , worldY + quad.mHeight, worldZ + quad.mWidth};

        quadNormals   = { -1.0f, 0.0f, 0.0f,
                          -1.0f, 0.0f, 0.0f,
                          -1.0f, 0.0f, 0.0f,
        
                          -1.0f, 0.0f, 0.0f,
                          -1.0f, 0.0f, 0.0f,
                          -1.0f, 0.0f, 0.0f};

        glm::vec2 texLoc;
        glm::vec2 plus(1.0f * quad.mWidth, 1.0f * quad.mHeight);

        quadTexCoords = {  texLoc.x         ,  texLoc.y         ,
                           texLoc.x + plus.x,  texLoc.y + plus.y,
                           texLoc.x         ,  texLoc.y + plus.y,
        
                           texLoc.x         ,  texLoc.y         ,
                           texLoc.x + plus.x,  texLoc.y         ,
                           texLoc.x + plus.x,  texLoc.y + plus.y};

        allQuadPositions.insert(allQuadPositions.end(), quadPositions.begin(), quadPositions.end());
        allQuadNormals.insert(allQuadNormals.end(), quadNormals.begin(), quadNormals.end());
        allQuadTexCoords.insert(allQuadTexCoords.end(), quadTexCoords.begin(), quadTexCoords.end());
    }

    for(auto& quad : rightQuads)
    {
        quadPositions.clear();
        quadNormals.clear();
        quadTexCoords.clear();

        float worldX = quad.mDepth + chunkOffset.x;
        float worldY = quad.mY + chunkOffset.y;
        float worldZ = quad.mX + chunkOffset.z;

        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);
        allTextureIndices.push_back(quad.mType);

        quadPositions = { worldX              , worldY,                worldZ,
                          worldX              , worldY + quad.mHeight, worldZ,
                          worldX              , worldY + quad.mHeight, worldZ + quad.mWidth,
                            
                          worldX              , worldY,                worldZ,
                          worldX              , worldY + quad.mHeight, worldZ + quad.mWidth,
                          worldX              , worldY,                worldZ + quad.mWidth};

        quadNormals   = { 1.0f, 0.0f, 0.0f,
                          1.0f, 0.0f, 0.0f,
                          1.0f, 0.0f, 0.0f,
        
                          1.0f, 0.0f, 0.0f,
                          1.0f, 0.0f, 0.0f,
                          1.0f, 0.0f, 0.0f};

        glm::vec2 texLoc;
        glm::vec2 plus(1.0f * quad.mWidth, 1.0f * quad.mHeight);

        quadTexCoords = {  texLoc.x         ,  texLoc.y         ,
                           texLoc.x         ,  texLoc.y + plus.y,
                           texLoc.x + plus.x,  texLoc.y + plus.y,
        
                           texLoc.x         ,  texLoc.y         ,
                           texLoc.x + plus.x,  texLoc.y + plus.y,
                           texLoc.x + plus.x,  texLoc.y         };

        allQuadPositions.insert(allQuadPositions.end(), quadPositions.begin(), quadPositions.end());
        allQuadNormals.insert(allQuadNormals.end(), quadNormals.begin(), quadNormals.end());
        allQuadTexCoords.insert(allQuadTexCoords.end(), quadTexCoords.begin(), quadTexCoords.end());
    }
    
    //printf("Generated %u vertices and %u indices\n", verts, indices);
    //After stuff has been added, you have to update the gpu vbo data.

    newModel.model.addVertexArray(ModelAttribute::POSITIONS, allQuadPositions);
    newModel.model.addVertexArray(ModelAttribute::NORMALS, allQuadNormals);
    newModel.model.addVertexArray(ModelAttribute::TEXCOORDS, allQuadTexCoords);
    newModel.textureIndices = allTextureIndices;

    return newModel;
}
