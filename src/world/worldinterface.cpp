#include "worldinterface.h"
#include "rendering/renderer.h"
    WorldInterface::WorldInterface(World& dimension, EntitySystem& entitySystem)
:   mWorld(dimension),
    mEntitySystem(entitySystem)
{

}

VoxelType WorldInterface::getVoxelTypeInt(int x, int y, int z) const
{
    // int chunkWidth = 16;
    ChunkCoordinate chunkCoordinate = worldToChunkInt(x, y, z);

    // int xNegative = (int)(x - chunkWidth) / chunkWidth;
    // int yNegative = (int)(y - chunkWidth) / chunkWidth;
    // int zNegative = (int)(z - chunkWidth) / chunkWidth;

    // if(xNegative < 0) x += (-xNegative + 1) * chunkWidth;
    // if(yNegative < 0) y += (-yNegative + 1) * chunkWidth;
    // if(zNegative < 0) z += (-zNegative + 1) * chunkWidth;

    VoxelCoordinate voxelCoordinate = worldToChunkVoxel(x, y, z);//VoxelCoordinate((x + (x) % 16,(y) % 16,(z) % 16);

    const Region& landscape = mWorld.getLandscape();

    if(landscape.chunkIsLoaded(chunkCoordinate))
    {
        return landscape.getChunk(chunkCoordinate).getVoxelType(voxelCoordinate);
    }
    else
    {
        return 0;
    }
}
VoxelType WorldInterface::getVoxelType(const VoxelWorldCoordinate coord) const{
    return getVoxelTypeInt(coord.x, coord.y, coord.z);
}


VoxelType WorldInterface::getVoxelType(float x, float y, float z) const
{
    ChunkCoordinate chunkCoordinate = worldToChunk(x, y, z);
    VoxelCoordinate voxelCoordinate = worldToChunkVoxel(x, y, z);

    const Region& landscape = mWorld.getLandscape();

    if(landscape.chunkIsLoaded(chunkCoordinate))
    {
        return landscape.getChunk(chunkCoordinate).getVoxelType(voxelCoordinate);
    }
    else
    {
        return 0;
    }
}

VoxelType WorldInterface::getVoxelType(const glm::vec3& position) const
{
    return getVoxelType(position.x, position.y, position.z);
}

bool WorldInterface::getVoxelAtRay(const glm::vec3& position, const glm::vec3& direction, const float maxDistance, int& hitFace, VoxelWorldCoordinate& hitBlock ) const
{
    return getVoxelAtRay(position.x, position.y, position.z, direction.x, direction.y, direction.z, maxDistance, hitFace, hitBlock);
}

bool WorldInterface::getVoxelAtRay(float ox, float oy, float oz, float dx, float dy, float dz, const float maxDistance, int& hitFace, VoxelWorldCoordinate& hitBlock)  const
{
    int ip[3];
    ip[0] = glm::floor(ox);
    ip[1] = glm::floor(oy);
    ip[2] = glm::floor(oz);

    glm::vec3 bp = glm::fract(glm::vec3(ox, oy, oz));
    VoxelCoordinate chunkCoordinate = worldToChunkVoxel(ip[0], ip[1], ip[2]);

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
    if(dx > 0){
        bounds.x = 1.0f;
        enterFaces[0] = FACE_LEFT;
    }else{
        enterFaces[0] = FACE_RIGHT;
    }
    if(dy > 0){
        bounds.y = 1.0f;
        enterFaces[1] = FACE_BOTTOM;
    }else{
        enterFaces[1] = FACE_TOP;
    }
    if(dz > 0){
        bounds.z = 1.0f;
        enterFaces[2] = FACE_FRONT;
    }else{
        enterFaces[2] = FACE_BACK;
    }
    float ix, iy, iz;
    while(steps < 256){//Able to look 256 blocks away!
        glm::vec3 distInBlock = bounds - bp;

        glm::vec3 poop = distInBlock / d;

        int mini = 0;
        float mind = poop.x;

        if(mind > poop.y){
            mind = poop.y;
            mini = 1;
        }
        if(mind > poop.z){
            mind = poop.z;
            mini = 2;
        }

        float lengthToNextBlock = 0.1f;
        lengthToNextBlock = mind + 0.01f;

        vtype = getVoxelTypeInt(
                ip[0], 
                ip[1], 
                ip[2]);

        if(vtype != (uint16_t)0) 
            break;

        bp += d * lengthToNextBlock;
        distanceTravelled += lengthToNextBlock;
        if(bp[mini] >= 1.f){
            ip[mini] ++;
            bp[mini] = 0.f;
        }else{
            ip[mini] --;
            bp[mini] = 1.f;
        }

        hitFace = enterFaces[mini];
        steps ++;

        if(distanceTravelled > maxDistance){
            return false;
        }
    }

    glm::vec3 block = glm::vec3(ip[0] , ip[1] , ip[2]);
    if(steps == 256){ return false; }
    hitBlock = VoxelWorldCoordinate(ip[0], ip[1],  ip[2]);
    return true;
}

fea::WeakEntityPtr WorldInterface::createEntity(const std::string& scriptType, const glm::vec3& position)
{
    return mEntitySystem.createEntity(scriptType, position);
}

ChunkList WorldInterface::getChunkList() const
{
    return mWorld.getLandscape().getChunkList();
}

EntityCreator WorldInterface::getEntityCreator() const
{
    return mEntitySystem.getEntityCreator();
}
