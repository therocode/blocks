#include "collisioncontroller.h"
#include "../../world/worldinterface.h"
#include <iostream>

CollisionController::CollisionController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface), mBus(bus)
{
    mBus.addMessageSubscriber<EntityMoveRequestedMessage>(*this);
}

CollisionController::~CollisionController()
{
    mBus.removeMessageSubscriber<EntityMoveRequestedMessage>(*this);
}

void CollisionController::inspectEntity(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->hasAttribute("position") &&
            locked->hasAttribute("velocity") &&
            locked->hasAttribute("on_ground") &&
            locked->hasAttribute("hitbox"))
    {
        mEntities.emplace(locked->getId(), entity);
    }
}

void CollisionController::onFrame(int dt)
{
    for(auto wEntity : mEntities)
    {
        fea::EntityPtr entity = wEntity.second.lock();
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
            if(!AABBOnGround(a)){
                entity->setAttribute<bool>("on_ground", false);
                mBus.sendMessage<EntityOnGroundMessage>(EntityOnGroundMessage(entity->getId(), false));
            }
        }else{
            if(AABBOnGround(a) && glm::abs(velocity.y) <= 0.001f){
                entity->setAttribute<bool>("on_ground", true);
                mBus.sendMessage<EntityOnGroundMessage>(EntityOnGroundMessage(entity->getId(), true));
            }
        }
    }
}

void CollisionController::handleMessage(const EntityMoveRequestedMessage& message)
{
    fea::EntityId id;
    glm::vec3 requestedPosition;
    glm::vec3 approvedPosition;

    std::tie(id, requestedPosition) = message.data;

    approvedPosition = requestedPosition;

    glm::vec3 move = glm::vec3(999, 999, 999);
    float moveY = 999;
    float moveZ = 999;

    fea::EntityPtr entity =  mEntities.at(id).lock();
    glm::vec3 oldPosition = entity->getAttribute<glm::vec3>("position");
    glm::vec3 size = entity->getAttribute<glm::vec3>("hitbox");
    AABB a;

    glm::vec3 velocity;

    a.width  = size.x;
    a.height = size.y;
    a.depth  = size.z;
    a.x = oldPosition.x - size.x * 0.5f;
    a.y = oldPosition.y - size.y * 0.5f;
    a.z = oldPosition.z - size.z * 0.5f;
    oldPosition += pushOutFromBlocks(a);

    glm::vec3 v = approvedPosition - oldPosition;

    glm::vec3 ignoreAxis = glm::vec3(0);
    float n = 0.0;
    int steps = 0;
    VoxelWorldCoordinate currentHitBlock;
    while(n < 1.0f && (steps < 3)){
        steps++;
        glm::ivec3 normal = glm::ivec3(0);
        a.x = oldPosition.x - size.x * 0.5f;
        a.y = oldPosition.y - size.y * 0.5f;
        a.z = oldPosition.z - size.z * 0.5f;
        n = sweepAroundAABB(a, v, normal, currentHitBlock, ignoreAxis);
        //Renderer::sDebugRenderer.drawBox(a.x + a.width*0.5f, a.y + a.height*0.5f, a.z + a.depth*0.5f, a.width  + 0.001f, a.height + 0.001f, a.depth + 0.001f, DebugRenderer::ORANGE);

        if(n < 1.f )
        {
            float moveLen = glm::length(approvedPosition - oldPosition);
            for(int i = 0; i < 3; i++){
                if(normal[i] != 0){
                    ignoreAxis[i] = 1.0;
                    break;
                }
            }
            float e = 0.001f;
            approvedPosition = oldPosition + v * n;//* glm::max(n - 0.08f / moveLen, 0.0f);
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
            }else if(normal.y != 0){
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
            }else{
                printf("What\n");
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
    mBus.sendMessage<EntityMovedMessage>(EntityMovedMessage(id, requestedPosition, approvedPosition));
}

glm::vec3 CollisionController::pushOutFromBlocks(const AABB& _a, float maxMove)
{
    AABB a, b;
    a = _a;
    a.x = a.y = a.z = 0.0f;
    glm::vec3 resultVec = glm::vec3(0.0f);

    int sx = 1, sy = 2, sz = 1;
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

                VoxelWorldCoordinate coord(b.x, b.y, b.z);
                b.x -= _a.x;
                b.y -= _a.y;
                b.z -= _a.z;

                if(mWorldInterface.getVoxelType(coord) != 0)
                {
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

float CollisionController::sweepAroundAABB(const AABB _a, glm::vec3 velocity, glm::ivec3& outNormal, VoxelWorldCoordinate& hitBlock, const glm::vec3 ignoreAxis)
{
    AABB a, b;
    a = _a;
    a.x = a.y = a.z = 0.0f;

    int sx = 1, sy = 2, sz = 1;
    float n = 1.0f;
    float whatN = 1.0f;
    glm::ivec3 normal = glm::ivec3(0);
    float longest = 99999.f;
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

                VoxelWorldCoordinate coord(b.x, b.y, b.z);
                b.x -= _a.x;
                b.y -= _a.y;
                b.z -= _a.z;

                if(mWorldInterface.getVoxelType(coord) != 0)
                {
                    glm::ivec3 norm;
                    // renderDebugAABB(b, DebugRenderer::GREEN);

                    //A is the entity, B is block in world, v is newPosition - oldPosition. Function should set norm to a normal on which face it collided. returns depth, which is between 0 and 1.
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
                        VoxelWorldCoordinate nc = coord;
                        if(norm[axis] > 0)
                            nc[axis] += 1;
                        else
                            nc[axis] -= 1;
                        if(mWorldInterface.getVoxelType(nc) != 0){
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

bool CollisionController::AABBOnGround(AABB a)
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

            if(mWorldInterface.getVoxelType(pos) != 0){
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

void CollisionController::removeEntity(fea::EntityId id)
{
    mEntities.erase(id);
}

bool CollisionController::checkIfOnGround(fea::EntityPtr entity)
{
    glm::vec3 currentVelocity = entity->getAttribute<glm::vec3>("velocity");
    bool isOnGround = entity->getAttribute<bool>("on_ground");

    if(glm::abs(currentVelocity.y) < 0.6 && !isOnGround)
    {
        entity->setAttribute<bool>("on_ground", true);
        mBus.sendMessage<EntityOnGroundMessage>(EntityOnGroundMessage(entity->getId(), true));
        return true;
    }
    return false;
}
