#include "gameinterface.hpp"
#include "rendering/renderer.hpp"
#include "entity/entitysystem.hpp"
#include "world/worldsystem.hpp"

GameInterface::GameInterface(const WorldSystem& worldSystem, const EntitySystem& entitySystem) :
    mWorldSystem(worldSystem),
    mEntitySystem(entitySystem)
{

}

//bool GameInterface::getVoxelAtRay(WorldId worldId, float ox, float oy, float oz, float dx, float dy, float dz, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock)  const
bool GameInterface::getVoxelAtRay(WorldId worldId, const glm::vec3& position, const glm::vec3& direction, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock) const
{
    VoxelCoord voxel = worldToVoxel(position);

    glm::vec3 bp = glm::fract(position);
    ChunkVoxelCoord chunkCoordinate = voxelToChunkVoxel(voxel);
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
        enterFaces[0] = FACE_LEFT;
    }
    else
    {
        enterFaces[0] = FACE_RIGHT;
    }
    if(d.y > 0)
    {
        bounds.y = 1.0f;
        enterFaces[1] = FACE_BOTTOM;
    }
    else
    {
        enterFaces[1] = FACE_TOP;
    }
    if(d.z > 0)
    {
        bounds.z = 1.0f;
        enterFaces[2] = FACE_FRONT;
    }
    else
    {
        enterFaces[2] = FACE_BACK;
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

        vtype = mWorldSystem.getWorld(worldId).getVoxelType(voxel);

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

const WorldSystem& GameInterface::getWorldSystem() const
{
    return mWorldSystem;
}

const EntitySystem& GameInterface::getEntitySystem() const
{
    return mEntitySystem;
}
