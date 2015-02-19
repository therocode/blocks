#include "collisioncontroller.hpp"
#include "../../gameinterface.hpp"
#include "../../world/worldsystem.hpp"

CollisionController::CollisionController(fea::MessageBus& bus, GameInterface& gameInterface) :
    EntityController(bus), mBus(bus),
    mGameInterface(gameInterface)
{
    subscribe(mBus, *this);
}

bool CollisionController::keepEntity(fea::WeakEntityPtr entity) const
{
    fea::EntityPtr locked = entity.lock();

    return locked->hasAttribute("position") &&
            locked->hasAttribute("velocity") &&
            locked->hasAttribute("acceleration") &&
            locked->hasAttribute("on_ground") &&
            locked->hasAttribute("current_world") &&
            locked->hasAttribute("hitbox");
}

void CollisionController::handleMessage(const FrameMessage& message)
{
    int32_t dt = message.deltaTime;

    for(auto wEntity : mEntities)
    {
        fea::EntityPtr entity = wEntity.second.lock();
        WorldId worldId = entity->getAttribute<WorldId>("current_world");
        bool isOnGround = entity->getAttribute<bool>("on_ground");
        glm::vec3 position = entity->getAttribute<glm::vec3>("position");
        glm::vec3 size = entity->getAttribute<glm::vec3>("hitbox");
        glm::vec3 velocity = entity->getAttribute<glm::vec3>("velocity");

        AABB a;

        a.x = position.x - size.x * 0.5f;
        a.y = position.y - size.y * 0.5f;
        a.z = position.z - size.z * 0.5f;

        a.width = size.x;
        a.height = size.y;
        a.depth = size.z;

        if(isOnGround){
            if(!AABBOnGround(worldId, a)){
                entity->setAttribute<bool>("on_ground", false);
                mBus.send(EntityOnGroundMessage{entity->getId(), false});
            }
        }else{
            if(AABBOnGround(worldId, a) && glm::abs(velocity.y) <= 0.001f){
                entity->setAttribute<bool>("on_ground", true);
                mBus.send(EntityOnGroundMessage{entity->getId(), true});
            }
        }
    }
}

void CollisionController::handleMessage(const EntityMoveRequestedMessage& received)
{
    fea::EntityId id = received.id;
    glm::vec3 requestedPosition = received.newPosition;
    glm::vec3 approvedPosition;

    approvedPosition = requestedPosition;

    glm::vec3 move = glm::vec3(999, 999, 999);
    float moveY = 999;
    float moveZ = 999;

    fea::EntityPtr entity =  mEntities.at(id).lock();
    WorldId worldId = entity->getAttribute<WorldId>("current_world");
    glm::vec3 oldPosition = entity->getAttribute<glm::vec3>("position");
    const glm::vec3 oldPositionRetained = oldPosition;
    glm::vec3 size = entity->getAttribute<glm::vec3>("hitbox");
    AABB a;

    glm::vec3 velocity;

    a.width  = size.x;
    a.height = size.y;
    a.depth  = size.z;
    a.x = oldPosition.x - size.x * 0.5f;
    a.y = oldPosition.y - size.y * 0.5f;
    a.z = oldPosition.z - size.z * 0.5f;
    oldPosition += pushOutFromBlocks(worldId, a, 0.05f);

    glm::vec3 v = approvedPosition - oldPosition;

    glm::vec3 ignoreAxis = glm::vec3(0);
    float n = 0.0;
    int steps = 0;
    VoxelCoord currentHitBlock;
    AABB b;
    b.width = 1.0f;
    b.height = 1.0f;
    while(n < 1.0f && (steps < 3)){
        steps++;
        glm::ivec3 normal = glm::ivec3(0);
        a.x = oldPosition.x - size.x * 0.5f;
        a.y = oldPosition.y - size.y * 0.5f;
        a.z = oldPosition.z - size.z * 0.5f;
        glm::vec3 c(0);
        n = sweepAroundAABB(worldId, a, v, normal, currentHitBlock, c);
        //Renderer::sDebugRenderer.drawBox(a.x + a.width*0.5f, a.y + a.height*0.5f, a.z + a.depth*0.5f, a.width  + 0.001f, a.height + 0.001f, a.depth + 0.001f, DebugRenderer::ORANGE);

        if(n < 1.f )
        {
            const auto& chunks = mGameInterface.getWorldSystem().getWorldVoxels(worldId);
            const auto chunk = chunks.find(VoxelToChunk::convert(currentHitBlock));

            int32_t blockType = 0;

            if(chunk != chunks.end())
                blockType = chunk->second.getVoxelType(VoxelToChunkVoxel::convert(currentHitBlock));

            float moveLen = glm::length(approvedPosition - oldPosition);

            b.x = currentHitBlock.x;
            b.y = currentHitBlock.y;
            b.z = currentHitBlock.z;

            float e = 0.001f;
            approvedPosition = oldPosition + v * n;//* glm::max(n - 0.08f / moveLen, 0.0f);
            if(normal.y != 0){
                ignoreAxis.y = 1.0f;
                if(normal.y > 0){
                    moveLen = (float)currentHitBlock.y + 1.0f + a.height * 0.5f + e - approvedPosition.y;
                    //approvedPosition.x = (float)currentHitBlock.x + 1.0f + a.width * 0.5f + e;
                }else{
                    moveLen = (float)currentHitBlock.y - a.height * 0.5f - e - approvedPosition.y; 
                    //approvedPosition.x = (float)currentHitBlock.x - a.width * 0.5f - e;
                }
                if(glm::abs(moveLen) < 0.1f){
                    approvedPosition.y += moveLen;
                }
            }else{
                if(blockType == 21)
                    b.height = 0.5f;
                else
                    b.height = 1.0f;

                if(b.max(1) - a.min(1) <= 1.0f){
                    if(AABBOnGround(worldId, a))
                    {
                        AABB aa = a;
                        float newY = b.max(1) + 0.01f;// + size.y * 0.5f
                        if(normal.x != 0)
                        {
                            aa.x += v.x;
                        }else
                        {
                            aa.z += v.z;
                        }
                        aa.y = newY;
                        if(!testAABBWorld(worldId, aa)){
                            if(normal.x != 0)
                                oldPosition.x += v.x * 0.1f;
                            else
                                oldPosition.z += v.z * 0.1f;
                            oldPosition.y = newY + size.y * 0.5f;
                            //oldPosition.y += b.height + 0.01f;
                            continue; 
                        }
                    }
                }
                if(normal.x != 0){
                    ignoreAxis.x = 1.0f;
                    if(normal.x > 0){
                        moveLen = (float)currentHitBlock.x + 1.0f + a.width * 0.5f + e - approvedPosition.x;
                        //approvedPosition.x = (float)currentHitBlock.x + 1.0f + a.width * 0.5f + e;
                    }else{
                        moveLen = (float)currentHitBlock.x - a.width * 0.5f - e - approvedPosition.x; 
                        //approvedPosition.x = (float)currentHitBlock.x - a.width * 0.5f - e;
                    }
                    if(glm::abs(moveLen) < 0.1f){
                        approvedPosition.x += moveLen;
                    }
                }else if(normal.z != 0){
                    ignoreAxis.z = 1.0f;
                    if(normal.z > 0){
                        moveLen = (float)currentHitBlock.z + 1.0f + a.depth * 0.5f + e - approvedPosition.z;
                        //approvedPosition.x = (float)currentHitBlock.x + 1.0f + a.width * 0.5f + e;
                    }else{
                        moveLen = (float)currentHitBlock.z - a.depth * 0.5f - e - approvedPosition.z; 
                        //approvedPosition.x = (float)currentHitBlock.x - a.width * 0.5f - e;
                    }
                    if(glm::abs(moveLen) < 0.1f){
                        approvedPosition.z += moveLen;
                    }
                }
            }
            for(int i = 0; i < 3; i++){
                if(normal[i] != 0){
                    ignoreAxis[i] = 1.0;
                    break;
                }
            }
            oldPosition = approvedPosition;

            velocity = mEntities.at(id).lock()->getAttribute<glm::vec3>("velocity");
            glm::vec3 acceleration = mEntities.at(id).lock()->getAttribute<glm::vec3>("acceleration");

            float remainingTime = 1.0f - n;
            float magn = glm::length(velocity) * remainingTime;
            glm::vec3 c = v;//(glm::vec3(1.f) - glm::abs(normal)) * v;
            if(normal.x != 0)c.x = 0;
            if(normal.y != 0)c.y = 0;
            if(normal.z != 0)c.z = 0;

            glm::vec3 additionalVelocity = c * remainingTime;
            approvedPosition += additionalVelocity;

            //velocity += additionalVelocity;
            //velocity is either 0 or 1, depending if it collided or not. when all is working it should use the normal to do stuff.
            //printf("noral: %f, %f, %f\n", normal.x, normal.y, normal.z);
            //velocity     *= glm::vec3(1.0) - glm::abs(normal);
            if(normal.x != 0)velocity.x = 0;
            if(normal.y != 0)velocity.y = 0;
            if(normal.z != 0)velocity.z = 0;
            // acceleration *= glm::vec3(1.0) - glm::abs(normal);
            v = approvedPosition - oldPosition;
            entity->setAttribute<glm::vec3>("velocity", velocity);
            entity->setAttribute<glm::vec3>("acceleration", acceleration);
        }else
        {
            approvedPosition = oldPosition + v;
        }
    }
    entity->setAttribute<glm::vec3>("position", approvedPosition);
    mBus.send(EntityMovedMessage{id, worldId, oldPositionRetained, approvedPosition});
}

bool CollisionController::testAABBWorld(WorldId worldId, const AABB& a) const{
    AABB b;
    b.width = b.depth = 1.0f;
    int sx = a.width*2.f + 0.5f, sy = a.height*2.f + 0.5f, sz = a.depth*2.f + 0.5f;
    //Loop througha cube of blocks and check if they are passableor not
    for(float x = -sx; x <= sx; x++)
    {
        for(float y = -sy; y <= sy + 1; y++)
        {
            for(float z = -sz; z <= sz; z++)
            {
                glm::vec3 cubePos = glm::vec3(a.x, a.y, a.z) + glm::vec3(x, y, z);
                //set position of aabb B
                b.x = glm::floor(cubePos.x);
                b.y = glm::floor(cubePos.y);
                b.z = glm::floor(cubePos.z);

                VoxelCoord coord(b.x, b.y, b.z);

                int blockType = 0;
                
                const auto& chunks = mGameInterface.getWorldSystem().getWorldVoxels(worldId);
                const auto chunk = chunks.find(VoxelToChunk::convert(coord));
                if(chunk != chunks.end())
                    blockType = chunk->second.getVoxelType(VoxelToChunkVoxel::convert(coord));

                if(blockType != 0)
                {
                    if(blockType == 21)
                        b.height = 0.5f;
                    else
                        b.height = 1.0f;

                    if(testAABBAABB(a, b))
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;

}

glm::vec3 CollisionController::pushOutFromBlocks(WorldId worldId, const AABB& _a, float maxMove)
{
    AABB a, b;
    a = _a;
    a.x = a.y = a.z = 0.0f;
    glm::vec3 resultVec = glm::vec3(0.0f);

    int sx = _a.width*2.f, sy = _a.height*2.f, sz = _a.depth*2.f;
    float maxMoveSqrd = maxMove * maxMove;
    //Loop througha cube of blocks and check if they are passableor not
    for(float x = -sx; x <= sx; x++)
    {
        for(float y = -sy; y <= sy + 1; y++)
        {
            for(float z = -sz; z <= sz; z++)
            {
                glm::vec3 cubePos = glm::vec3(_a.x, _a.y, _a.z) + glm::vec3(x, y, z);
                //set position of aabb B
                b.x = glm::floor(cubePos.x);
                b.y = glm::floor(cubePos.y);
                b.z = glm::floor(cubePos.z);

                VoxelCoord coord(b.x, b.y, b.z);
                b.x -= _a.x;
                b.y -= _a.y;
                b.z -= _a.z;

                const auto& chunks = mGameInterface.getWorldSystem().getWorldVoxels(worldId);
                const auto chunk = chunks.find(VoxelToChunk::convert(coord));
                int blockType = 0;
                
                if(chunk != chunks.end())
                    blockType = chunk->second.getVoxelType(VoxelToChunkVoxel::convert(coord));

                if(blockType != 0)
                {
                    if(blockType == 21)
                        b.height = 0.5f;
                    else
                        b.height = 1.0f;

                    glm::vec3 pushNorm;
                    testAABBAABB(a, b, pushNorm);
                    if(glm::length2(pushNorm) < maxMoveSqrd)
                    {
                        a.x  += pushNorm.x;
                        a.y  += pushNorm.y;
                        a.z  += pushNorm.z;

                        resultVec += pushNorm;
                    }
                }
            }
        }
    }
    return resultVec;
}

float CollisionController::sweepAroundAABB(WorldId worldId, const AABB _a, glm::vec3 velocity, glm::ivec3& outNormal, VoxelCoord& hitBlock, const glm::vec3 ignoreAxis)
{
    AABB a, b;
    a = _a;
    a.x = a.y = a.z = 0.0f;

    int sx = _a.width*2.f, sy = _a.height*2.f, sz = _a.depth*2.f;
    float n = 1.0f;
    float whatN = 1.0f;
    glm::ivec3 normal = glm::ivec3(0);
    float longest = 99999.f;
    const auto& chunks = mGameInterface.getWorldSystem().getWorldVoxels(worldId);
    //Loop througha cube of blocks and check if they are passableor not
    for(float x = -sx; x <= sx; x++)
    {
        for(float y = -sy; y <= sy + 1; y++)
        {
            for(float z = -sz; z <= sz; z++)
            {
                glm::vec3 cubePos = glm::vec3(_a.x + velocity.x, _a.y + velocity.y, _a.z + velocity.z) + glm::vec3(x, y, z);
                //set position of aabb B
                b.x = glm::floor(cubePos.x);
                b.y = glm::floor(cubePos.y);
                b.z = glm::floor(cubePos.z);

                VoxelCoord coord(b.x, b.y, b.z);
                b.x -= _a.x;
                b.y -= _a.y;
                b.z -= _a.z;

                int blockType = 0;
                const auto chunk = chunks.find(VoxelToChunk::convert(coord));
                
                if(chunk != chunks.end())
                    blockType = chunk->second.getVoxelType(VoxelToChunkVoxel::convert(coord));

                if(blockType != 0)
                {
                    if(blockType == 21)
                        b.height = 0.5f;
                    else
                        b.height = 1.0f;
                    glm::ivec3 norm;
                    // renderDebugAABB(b, DebugRenderer::GREEN);

                    //A is the entity, B is block in world, v is newPosition - oldPosition. 
                    //Function should set norm to a normal on which face it collided. returns depth, which is between 0 and 1.
                    float nn = sweepAABB(a, b, velocity, glm::vec3(0.f), norm);

                    //If depth is shallower than before, set the new depth to the new value.
                    int axis = 0;
                    for(int i = 0; i < 3; i++)
                    {
                        if(norm[i] != 0){
                            axis = i;
                            break;
                        }
                    }
                    if(ignoreAxis[axis] != 0.0) continue;
                    if(nn < n){
                        //Check if collision face is visible at all. if not, ignore this collision.
                        VoxelCoord nc = coord;
                        if(norm[axis] > 0)
                            nc[axis] += 1;
                        else
                            nc[axis] -= 1;

                        int voxelType = 0;
                        const auto chunk = chunks.find(VoxelToChunk::convert(nc));
                        
                        if(chunk != chunks.end())
                            voxelType = chunk->second.getVoxelType(VoxelToChunkVoxel::convert(nc));
                        if(voxelType != 0){
                        }
                        hitBlock = coord;
                        n = nn;
                        normal = norm;
                    }
                }
            }
        }
    }
    if(whatN < n){
        n = whatN;
    }
    if(n < 1.0)
    {
        outNormal = normal;
        return n;
    }
    return 1.0;
}

bool CollisionController::AABBOnGround(WorldId worldId, AABB a)
{
    float s = 0.05f;
    AABB b;
    //a.x += a.width * s;
    //a.z += a.depth * s;
    //a.width *= 1.0f - s*2.0f;
    //a.depth *= 1.0f - s*2.0f;
    a.y -= 0.02f;
    a.height = 0;
    float y = a.y;
    glm::vec3 pos;
    pos.y = y;
    for(float x = 0; x <= a.width; x += a.width / 4.f)
        for(float z = 0; z <= a.depth; z += a.depth / 4.f)
        {
            pos.x = x + a.x;
            pos.z = z + a.z;
            int32_t voxelType = 0;
            const auto& chunks = mGameInterface.getWorldSystem().getWorldVoxels(worldId);
            const auto chunk = chunks.find(WorldToChunk::convert(pos));

            if(chunk != chunks.end())
                voxelType = chunk->second.getVoxelType(WorldToChunkVoxel::convert(pos));
            if(voxelType != 0){
                b.x = glm::floor(pos.x);
                b.y = glm::floor(pos.y);
                b.z = glm::floor(pos.z);

                b.x = (int)b.x;
                b.y = (int)b.y;
                b.z = (int)b.z;
                if(testAABBAABB(a, b)){
                    return true;
                }
            }
        }
    return false;

}

bool CollisionController::checkIfOnGround(fea::EntityPtr entity)
{
    glm::vec3 currentVelocity = entity->getAttribute<glm::vec3>("velocity");
    bool isOnGround = entity->getAttribute<bool>("on_ground");

    if(glm::abs(currentVelocity.y) < 0.6 && !isOnGround)
    {
        entity->setAttribute<bool>("on_ground", true);
        mBus.send(EntityOnGroundMessage{entity->getId(), true});
        return true;
    }
    return false;
}
