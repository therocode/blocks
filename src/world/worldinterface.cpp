#include "worldinterface.hpp"
#include "rendering/renderer.hpp"
#include "world.hpp"

WorldInterface::WorldInterface(std::unordered_map<WorldId, std::unique_ptr<World>>& worlds, EntitySystem& entitySystem) :
    mWorlds(worlds),
    mEntitySystem(entitySystem)
{

}

VoxelType WorldInterface::getVoxelTypeInt(WorldId worldId, int x, int y, int z) const
{
    return mWorlds.at(worldId)->getVoxelType(VoxelCoord(x, y, z));
}

VoxelType WorldInterface::getVoxelType(WorldId worldId, const VoxelCoord coord) const
{
    return getVoxelTypeInt(worldId, coord.x, coord.y, coord.z);
}


VoxelType WorldInterface::getVoxelType(WorldId worldId, float x, float y, float z) const
{
    return getVoxelType(worldId, worldToVoxel(glm::vec3(x, y, z)));
}

VoxelType WorldInterface::getVoxelType(WorldId worldId, const glm::vec3& position) const
{
    return getVoxelType(worldId, worldToVoxel(position));
}

bool WorldInterface::getVoxelAtRay(WorldId worldId, const glm::vec3& position, const glm::vec3& direction, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock) const
{
    return getVoxelAtRay(worldId, position.x, position.y, position.z, direction.x, direction.y, direction.z, maxDistance, hitFace, hitBlock);
}

bool WorldInterface::getVoxelAtRay(WorldId worldId, float ox, float oy, float oz, float dx, float dy, float dz, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock)  const
{
    int ip[3];
    ip[0] = glm::floor(ox);
    ip[1] = glm::floor(oy);
    ip[2] = glm::floor(oz);

    glm::vec3 bp = glm::fract(glm::vec3(ox, oy, oz));
    ChunkVoxelCoord chunkCoordinate = voxelToChunkVoxel(worldToVoxel(glm::vec3(ip[0], ip[1], ip[2])));
    //printf("ip:%i, %i, %i\n", ip[0], ip[1], ip[2]);
    //printf("ip:%i, %i, %i\n", chunkCoordinate[0], chunkCoordinate[1], chunkCoordinate[2]);
    //printf("rp:%f, %f, %f\n", ox, oy, oz);
    //glm::vec3 ip = glm::vec3((int), (int)oy, (int)oz);

    glm::vec3 d = glm::vec3(dx, dy, dz);
    if(glm::length2(d) != 0)
        d = glm::normalize(d);
    else
        return false;

    glm::vec3 p = glm::vec3(ox, oy, oz);

    float distanceTravelled = 0.f;
    int steps = 0;
    uint16_t vtype = 0;
    glm::vec3 bounds = glm::vec3(0,0,0);
    int enterFaces[3];
    if(dx > 0)
    {
        bounds.x = 1.0f;
        enterFaces[0] = FACE_LEFT;
    }
    else
    {
        enterFaces[0] = FACE_RIGHT;
    }
    if(dy > 0)
    {
        bounds.y = 1.0f;
        enterFaces[1] = FACE_BOTTOM;
    }
    else
    {
        enterFaces[1] = FACE_TOP;
    }
    if(dz > 0)
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

        vtype = getVoxelTypeInt(worldId,
                ip[0], 
                ip[1], 
                ip[2]);

        if(vtype != (uint16_t)0) 
            break;

        bp += d * lengthToNextBlock;
        distanceTravelled += lengthToNextBlock;
        if(bp[mini] >= 1.f)
        {
            ip[mini] ++;
            bp[mini] = 0.f;
        }
        else
        {
            ip[mini] --;
            bp[mini] = 1.f;
        }

        hitFace = enterFaces[mini];
        steps ++;

        if(distanceTravelled > maxDistance)
        {
            return false;
        }
    }

    glm::vec3 block = glm::vec3(ip[0] , ip[1] , ip[2]);
    if(steps == 256)
    { 
        return false;
    }
    hitBlock = VoxelCoord(ip[0], ip[1],  ip[2]);
    return true;
}

fea::WeakEntityPtr WorldInterface::createEntity(const std::string& scriptType, const glm::vec3& position)
{
    return mEntitySystem.createEntity(scriptType, [&] (fea::EntityPtr e) 
            {
                e->setAttribute("position", position);
            });
}

ChunkReferenceMap WorldInterface::getChunkMap(WorldId worldId) const
{
    return mWorlds.at(worldId)->getChunkMap();
}

EntityCreator WorldInterface::getEntityCreator() const
{
    return mEntitySystem.getEntityCreator();
}

void WorldInterface::addHighlightEntity(WorldId worldId, fea::EntityId id, const ChunkCoord& coordinate)
{
    mWorlds.at(worldId)->addHighlightEntity(id, coordinate);
}

void WorldInterface::removeHighlightEntity(WorldId worldId, fea::EntityId id)
{
    mWorlds.at(worldId)->removeHighlightEntity(id);
}

void WorldInterface::moveHighlightEntity(WorldId worldId, fea::EntityId id, const ChunkCoord& coordinate)
{
    mWorlds.at(worldId)->moveHighlightEntity(id, coordinate);
}
