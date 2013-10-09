#include "vbocreator.h"
#include <vector>
#include <iostream>

ChunkVBO VBOCreator::generateChunkVBO(const Chunk& chunk) const
{
    ChunkVBO vbo;
    const VoxelTypeArray& voxelTypes = chunk.getVoxelTypes();

    std::vector<float> vertices;
    std::vector<float> normals;

    uint32_t chunkWidth = chunk.getWidth();
    for(uint32_t z = 0; z < chunkWidth; z++)
    {
        for(uint32_t y = 0; y < chunkWidth; y++)
        {
            for(uint32_t x = 0; x < chunkWidth; x++)
            {
                std::vector<float> toAdd = {x - 1.0f,
                                            y + 1.0f,
                                            z + 1.0f,
                                            
                                            x - 1.0f,
                                            y - 1.0f,
                                            z + 1.0f,
                                            
                                            x + 1.0f,
                                            y - 1.0f,
                                            z + 1.0f};

                vertices.insert(vertices.end(), toAdd.begin(), toAdd.end());

                toAdd.clear();
                toAdd = {-1.0f,
                          1.0f,
                          1.0f,
                                            
                         -1.0f,
                         -1.0f,
                          1.0f,
                                            
                          1.0f,
                         -1.0f,
                          1.0f};

                normals.insert(normals.end(), toAdd.begin(), toAdd.end());

                std::cout << "created triange\n";
            }
        }
    }

    vbo.setVertexData(vertices.size() * sizeof(float), &vertices[0]);
    vbo.setNormalData(normals.size() * sizeof(float), &normals[0]);
    std::cout << "vertices size is " << vertices.size() << "\n";

    return vbo;
}
