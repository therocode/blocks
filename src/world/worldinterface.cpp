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

    //const Region& landscape = mWorld.getLandscape();

    //if(landscape.chunkIsLoaded(chunkCoordinate))
    //{
    //    return landscape.getChunk(chunkCoordinate).getVoxelType(voxelCoordinate);
    //}
    //else
    //{
        return 0;
    //}
}
VoxelType WorldInterface::getVoxelType(const VoxelWorldCoordinate coord) const{
    return getVoxelTypeInt(coord.x, coord.y, coord.z);
}


VoxelType WorldInterface::getVoxelType(float x, float y, float z) const
{
    ChunkCoordinate chunkCoordinate = worldToChunk(x, y, z);
    VoxelCoordinate voxelCoordinate = worldToChunkVoxel(x, y, z);

    //const Region& landscape = mWorld.getLandscape();

    //if(landscape.chunkIsLoaded(chunkCoordinate))
    //{
    //    return landscape.getChunk(chunkCoordinate).getVoxelType(voxelCoordinate);
    //}
    //else
    //{
        return 0;
    //}
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
    int ip[3] = {(int)ox -(ox<0), (int)oy -(oy<0), (int)oz -(oz<0)};
    glm::vec3 bp = glm::fract(glm::vec3(ox, oy, oz));
    VoxelCoordinate chunkCoordinate = worldToChunkVoxel(ip[0], ip[1], ip[2]);
    // printf("ip:%i, %i, %i\n", ip[0], ip[1], ip[2]);
    // printf("ip:%i, %i, %i\n", chunkCoordinate[0], chunkCoordinate[1], chunkCoordinate[2]);
    // printf("rp:%f, %f, %f\n", ox, oy, oz);
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
    // printf("o: %i, %i, %i\n", ip[0], ip[1], ip[2]);
    while(steps < 256){//Able to look 256 blocks away!
        //glm::vec3 lp = glm::fract(p);
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
        // if(vtype != getVoxelType((float)ip[0] + 0.1f, (float)ip[1] + 0.1f, (float)ip[2] + 0.1f))
        // {
        // printf("not the same. this is wrong.\n");
        // VoxelCoordinate voxelCoordinate = worldToChunkVoxel(ip[0], ip[1], ip[2]);
        // VoxelCoordinate voxelCoordinate2 = worldToChunkVoxel((float)ip[0] + 0.1f, (float)ip[1] + 0.1f, (float)ip[2] + 0.1f);
        // voxelCoordinate -= voxelCoordinate2;
        // printf("diffaerecne chunk: %i, %i, %i\n", voxelCoordinate.x, voxelCoordinate.y, voxelCoordinate.z);
        // }
        // Renderer::sDebugRenderer.drawPoint(ip[0] + 0.5f, ip[1] + 0.5f, ip[2] + 0.5f, DebugRenderer::ORANGE);

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
    //	printf("steps:%i\n", steps);

    //printf("not the same. this is wrong.\n");
    // VoxelCoordinate voxelCoordinate = worldToChunkVoxel((int) -1, (int) -1, (int) -1);
    //VoxelCoordinate voxelCoordinate2 = worldToChunkVoxel(1, 0, -1);
    // voxelCoordinate -= voxelCoordinate2;
    //printf("diffaerecne chunk: %i, %i, %i\n", voxelCoordinate2.x, voxelCoordinate2.y, voxelCoordinate2.z);

    glm::vec3 block = glm::vec3(ip[0] , ip[1] , ip[2]);
    // printf("lookat block = %f, %f, %f\n",block.x, block.y, block.z);
    if(steps == 256){ return false; }
    hitBlock = VoxelWorldCoordinate(ip[0], ip[1],  ip[2]);
    return true;
    // return glm::vec3(ip[0] - (ip[0] < 0), ip[1] - (ip[1] < 0), ip[2] - (ip[2] < 0)) + glm::vec3(0.5f);
    // return glm::vec3((int)p.x - (p.x<0),(int)p.y - (p.y<0),(int)p.z - (p.z<0)) + glm::vec3(0.5f);
}

fea::WeakEntityPtr WorldInterface::createEntity(const std::string& scriptType, const glm::vec3& position)
{
    return mEntitySystem.createEntity(scriptType, position);
}

ChunkReferenceMap WorldInterface::getChunkList() const
{
    return mWorld.getChunkList();
}

EntityCreator WorldInterface::getEntityCreator() const
{
    return mEntitySystem.getEntityCreator();
}
