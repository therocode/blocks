#include "chunk.h"
#include <iostream>
#include <chrono>

uint32_t Chunk::totalTime = 0;
uint32_t Chunk::totalSize = 0;
uint32_t Chunk::timesGenerated = 0;

ChunkCoordinate worldToChunk(float x, float y, float z)
{
    return ChunkCoordinate(	floor(x / (float)chunkWidth), 
							floor(y / (float)chunkWidth), 
							floor(z / (float)chunkWidth));
}

ChunkCoordinate worldToChunk(const glm::vec3& position)
{
    return worldToChunk(position.x, position.y, position.z);
}

VoxelCoordinate worldToVoxel(float x, float y, float z)
{
    bool xNegative = x < 0.0f;
    bool yNegative = y < 0.0f;
    bool zNegative = z < 0.0f;

    return VoxelCoordinate((((int)x) % chunkWidth) + (xNegative?chunkWidth-1:0),
                           (((int)y) % chunkWidth) + (yNegative?chunkWidth-1:0),
                           (((int)z) % chunkWidth) + (zNegative?chunkWidth-1:0));
}

VoxelCoordinate worldToVoxel(const glm::vec3& position)
{
    return worldToVoxel(position.x, position.y, position.z);
}

Chunk::Chunk(const ChunkCoordinate& loc) : location(loc), compressed(false)
{
}

Chunk::Chunk(const ChunkCoordinate& loc, const VoxelTypeArray& types) : location(loc), voxelTypes(types), compressed(false)
{
}

void Chunk::setVoxelType(uint32_t x, uint32_t y, uint32_t z, VoxelType type)
{
    voxelTypes[x + z * chunkWidth + y * chunkWidthx2] = type;
}

void Chunk::setVoxelType(const VoxelCoordinate& voxel, VoxelType type)
{
    setVoxelType(voxel.x, voxel.y, voxel.z, type);
}

VoxelType Chunk::getVoxelType(uint32_t x, uint32_t y, uint32_t z) const
{
    return voxelTypes[x + z * chunkWidth + y * chunkWidthx2];
}

VoxelType Chunk::getVoxelType(const VoxelCoordinate& voxel) const
{
    return getVoxelType(voxel.x, voxel.y, voxel.z);
}

VoxelTypeArray& Chunk::getVoxelTypes()
{
    return voxelTypes;
}

const VoxelTypeArray& Chunk::getVoxelTypes() const
{
    return voxelTypes;
}

uint32_t Chunk::getWidth() const
{
    return chunkWidth;
}

const ChunkCoordinate& Chunk::getLocation() const
{
    return location;
}

void Chunk::compress()
{
    uint16_t currentType = voxelTypes[0];
    size_t index = 0;
    size_t voxelAmount = voxelTypes.size();
    uint16_t counter = 1;

    using namespace std::chrono;

    std::cout << "size before compression: " << sizeof(VoxelTypeArray) << "\n";
    high_resolution_clock::time_point now = high_resolution_clock::now();

    while(index < voxelAmount)
    {
        index++;

        uint16_t voxel;

        if(index != voxelAmount)
            voxel = voxelTypes[index];
        else
            voxel = -1;

        if(voxel == currentType)
        {
            counter++;
        }
        else
        {
            compressedData.push_back(currentType);
            compressedData.push_back(counter);

            counter = 1;
            currentType = voxel;
        }
    }

    high_resolution_clock::time_point then = high_resolution_clock::now();

    totalTime += duration_cast<microseconds>(then - now).count();
    timesGenerated++;
    totalSize += compressedData.size() * sizeof(uint16_t);

    std::cout << "size after compression: " << compressedData.size() * sizeof(uint16_t) << "\n";
    std::cout << "the compression process took " << duration_cast<microseconds>(then - now).count() << " microseconds\n";
    std::cout << "average compression is " << totalTime / timesGenerated << " microseconds and average size is " << totalSize / timesGenerated << "\n\n";
}
