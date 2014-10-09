#include "raycaster.hpp"
#include "worlddefines.hpp"
#include "chunkmap.hpp"

bool RayCaster::getVoxelAtRay(const ChunkMap& world, const glm::vec3& position, const glm::vec3& direction, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock)
{
    VoxelCoord voxel = WorldToVoxel::convert(position);

    glm::vec3 bp = glm::fract(position);
    ChunkVoxelCoord chunkCoordinate = VoxelToChunkVoxel::convert(voxel);
    //printf("ip:%i, %i, %i\n", ip[0], ip[1], ip[2]);
    //printf("ip:%i, %i, %i\n", chunkCoordinate[0], chunkCoordinate[1], chunkCoordinate[2]);
    //printf("rp:%f, %f, %f\n", ox, oy, oz);
    //glm::vec3 ip = glm::vec3((int), (int)oy, (int)oz);

    glm::vec3 p = position;

    glm::vec3 d = direction;
    if(glm::length2(d) != 0)
        d = glm::normalize(d);
    else
        return false;

    float distanceTravelled = 0.f;
    int steps = 0;
    uint16_t vtype = 0;
    glm::vec3 bounds = glm::vec3(0,0,0);
    int enterFaces[3];
    if(d.x > 0)
    {
        bounds.x = 1.0f;
        enterFaces[0] = CUBE_LEFT;
    }
    else
    {
        enterFaces[0] = CUBE_RIGHT;
    }
    if(d.y > 0)
    {
        bounds.y = 1.0f;
        enterFaces[1] = CUBE_BOTTOM;
    }
    else
    {
        enterFaces[1] = CUBE_TOP;
    }
    if(d.z > 0)
    {
        bounds.z = 1.0f;
        enterFaces[2] = CUBE_FRONT;
    }
    else
    {
        enterFaces[2] = CUBE_BACK;
    }
    float ix, iy, iz;
    while(steps < 256)
    {//Able to look 256 blocks away!
        glm::vec3 distInBlock = bounds - bp;

        glm::vec3 poop = distInBlock / d;

        int mini = 0;
        float mind = poop.x;

        if(mind > poop.y)
        {
            mind = poop.y;
            mini = 1;
        }
        if(mind > poop.z)
        {
            mind = poop.z;
            mini = 2;
        }

        float lengthToNextBlock = 0.1f;
        lengthToNextBlock = mind + 0.01f;

        vtype = 0;
        
        const auto chunk = world.find(VoxelToChunk::convert(voxel));
        if(chunk != world.end())
            vtype = chunk->second.getVoxelType(VoxelToChunkVoxel::convert(voxel));

        if(vtype != (uint16_t)0) 
            break;

        bp += d * lengthToNextBlock;
        distanceTravelled += lengthToNextBlock;
        if(bp[mini] >= 1.f)
        {
            voxel[mini] ++;
            bp[mini] = 0.f;
        }
        else
        {
            voxel[mini] --;
            bp[mini] = 1.f;
        }

        hitFace = enterFaces[mini];
        steps ++;

        if(distanceTravelled > maxDistance)
        {
            return false;
        }
    }

    if(steps == 256)
    { 
        return false;
    }
    hitBlock = voxel;
    return true;
}

