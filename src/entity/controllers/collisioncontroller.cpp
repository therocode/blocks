#include "collisioncontroller.h"
#include "../../world/worldinterface.h"
#include <iostream>
#include "rendering/renderer.h"

CollisionController::CollisionController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
    bus.addMessageSubscriber<EntityMoveRequestedMessage>(*this);
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

void CollisionController::onFrame()
{
    for(auto wEntity : mEntities)
    {
        fea::EntityPtr entity = wEntity.second.lock();

        if(entity->getAttribute<bool>("on_ground"))
        {
            glm::vec3 position = entity->getAttribute<glm::vec3>("position");

            if(mWorldInterface.getVoxelType(position + glm::vec3(0.f, -0.8f, 0.f)) == 0)
            {
                entity->setAttribute<bool>("on_ground", false);
                mBus.sendMessage<EntityOnGroundMessage>(EntityOnGroundMessage(entity->getId(), false));
            }
            else
            {
                entity->setAttribute<glm::vec3>("velocity", entity->getAttribute<glm::vec3>("velocity") * 0.8f);
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
    AABB a, b;

    a.x = oldPosition.x - size.x * 0.5f;
    a.y = oldPosition.y - size.y * 0.5f;
    a.z = oldPosition.z - size.z * 0.5f;

    a.width = size.x;
    a.height = size.y;
    a.depth = size.z;
    glm::vec3 v = requestedPosition - oldPosition;

    b.width =  1.f;
    b.height = 1.f;
    b.depth =  1.f;

    int sx = 2, sy = 2, sz = 2;
    float n = 1.0f;
    glm::vec3 normal = glm::vec3(0.f);
    //Loop througha cube of blocks and check if they are passableor not
    for(float x = -sx; x <= sx; x++)
    {
        //float x = 0, z = 0;
        for(float y = -sy; y <= sy; y++)
        {
            for(float z = -sz; z <= sz; z++)
            {
                // float x = 0, y = 0, z =0;
                glm::vec3 cubePos = glm::floor(glm::vec3(oldPosition.x, oldPosition.y, oldPosition.z)) + glm::vec3(x, y, z);

                if(mWorldInterface.getVoxelType(cubePos) != 0)
                {
                    glm::vec3 norm;
                    // v.x = 0; 
                    // v.y = -100; v.z = 0;
                    //set position of aabb B
                    //Might be something wrong here.
				
                    b.x = cubePos.x;                 
					b.y = cubePos.y;
                    b.z = cubePos.z;
					
					if(b.x <= 0) b.x --;
					if(b.y <= 0) b.y --;
					if(b.z <= 0) b.z --;
					
					
					Renderer::sDebugRenderer.drawCube(b.x + 0.5f, b.y + 0.5f, b.z + 0.5f, 1.02f, DebugRenderer::RED);
                    // printf("d: %f, %f, %f\n", cubePos.x - oldPosition.x, cubePos.y - oldPosition.y, cubePos.z - oldPosition.z);
                    //A is the entity, B is block in world, v is newPosition - oldPosition. Function should set norm to a normal on which face it collided. returns depth, which is between 0 and 1.
                    float nn = sweepAABB(a, b, v, norm);
                    // printf("nn:%f\n", nn);
                    //If depth is shallower than before, set the new depth to the new value.
                    if(nn < n && glm::abs(norm.y) > 0.001f){
                        n = nn;
                        normal = norm;
                    }
                }
            }
        }
    }
    if(n < 1.f)
    { 
        // printf("fucking shit:%f\n", n);
        //glm::floor(glm::vec3(oldPosition) ) + glm::vec3((float)x, (float)y, (float)z);
        // printf("a: %f, %f, %f\n", a.x, a.y, a.z);
        // printf("b: %f, %f, %f\n", cubePos.x, cubePos.y, cubePos.z);

        // if(cubePos.y < 50)

        // a.x = 0.f; a.y = 0.f; a.z = 0.f;
        // v.x = 100; v.z = 100;v.y = 100;
        // b.x = 50; b.z = 50; b.y = 50;
        // glm::vec3 norm;
        // float f = sweepAABB(a, b, v, norm);
        // printf("f: %f\n", f);

        approvedPosition = oldPosition + v * n;
        glm::vec3 velocity = mEntities.at(id).lock()->getAttribute<glm::vec3>("velocity");
        glm::vec3 acceleration = mEntities.at(id).lock()->getAttribute<glm::vec3>("acceleration");


        float remainingTime = 1.f - n;
        float magn = glm::length(velocity) * remainingTime;
        float collDot = glm::dot(normal, velocity);
        collDot = glm::clamp(collDot, -1.f, 1.f);
        glm::vec3 c = (glm::vec3(1.f) - glm::abs(normal)) * v;
        glm::vec3 additionalVelocity = c * remainingTime;
        approvedPosition += additionalVelocity;
        //velocity += additionalVelocity;
        //velocity is either 0 or 1, depending if it collided or not. when all is working it should use the normal to do stuff.
        //printf("noral: %f, %f, %f\n", normal.x, normal.y, normal.z);
        velocity     *= glm::vec3(1.0) - glm::abs(normal);
        //acceleration *= glm::vec3(1.0) - glm::abs(normal);

        entity->setAttribute<glm::vec3>("velocity", velocity);
        //entity->setAttribute<glm::vec3>("acceleration", acceleration);

        if(glm::abs(normal.y) > 0)
            checkIfOnGround(entity);
    }
    entity->setAttribute<glm::vec3>("position", approvedPosition);
    mBus.sendMessage<EntityMovedMessage>(EntityMovedMessage(id, requestedPosition, approvedPosition));
}

bool CollisionController::AABBAABB(const AABB a, const AABB b)
{

    if(a.x <= b.x + b.width && a.y <= b.y + b.height && a.z <= b.z + b.depth)
        if(a.x + a.width >= b.x && a.y + a.depth >= b.y && a.z + a.depth >= b.z)
            return true;

    return false;
}
float CollisionController::sweepAABB(const AABB a, const AABB b, const glm::vec3 v, glm::vec3& n)
{
    AABB newAABB = a;
    newAABB.x += v.x;
    newAABB.y += v.y;
    newAABB.z += v.z;

    if(!AABBAABB(newAABB, b))
    {
        n.x = n.y = n.z = 0;
        return 1.0;
    }
    
    float epsilon = 0.001f;
    float xEntry, yEntry, zEntry;
    float xExit,  yExit,  zExit;
    if(v.x > 0.0f)
    {
        xEntry = b.x - (a.x + a.width);
        xExit  = (b.x + b.width) - a.x;
    }else
    {
        xEntry = (b.x + b.width) - a.x;
        xExit  = b.x - (a.x + a.width);
    }

    if(v.y > 0.0f)
    {
        yEntry = b.y - (a.y + a.height);
        yExit  = (b.y + b.height) - a.y;
    }else
    {
        yEntry = (b.y + b.height) - a.y;
        yExit  = b.y - (a.y + a.height);
    }

    if(v.z > 0.0f)
    {
        zEntry = b.z - (a.z + a.depth);
        zExit  = (b.z + b.depth) - a.z;
    }else
    {
        zEntry = (b.z + b.depth) - a.z;
        zExit  = b.z - (a.z + a.depth);
    }
    //	printf("entry: %f, %f,%f\n", xEntry, yEntry, zEntry);
    float xs, ys, zs;
    float xe, ye, ze;
    float infinity = std::numeric_limits<float>::infinity();

    if(glm::abs(v.x) < epsilon)
    {
        xs = -infinity;
        xe =  infinity;
    }else
    {
        xs = xEntry / v.x;
        xe = xExit  / v.x;
    }

    if(glm::abs(v.y) < epsilon)
    {
        ys = -infinity;
        ye =  infinity;
    }else
    {
        ys = yEntry / v.y;
        ye = yExit  / v.y;
    }
    if(glm::abs(v.z) < epsilon)
    {
        zs = -infinity;
        ze =  infinity;
    }else
    {
        zs = zEntry / v.z;
        ze = zExit  / v.z;
    }

    float entry = xs;
    float exit  = xe;
    if(ys > entry) entry = ys;
    if(zs > entry) entry = zs;

    if(ye < exit)  exit = ye;
    if(ze < exit)  exit = ze;
    float l = glm::length(v) + 0.0001f;

    if(entry > exit)
    {
        n.x = n.y = n.z = 0.0f;
        //		printf("what\n");
        return 1.0f;
    }else if(xs < -epsilon && ys < -epsilon && zs < -epsilon)
    {
        n.x = n.y = n.z = 0.0f;
        //		printf("inside\n");
        return 1.0f;
    }else if(xs > 1.0f + epsilon && ys > 1.0f + epsilon && zs > 1.0f + epsilon)
    {
        n.x = n.y = n.z = 0.0f;
        //		printf("longer\n");
        return 1.0f;
    }
    else
    {
        // printf("entries: %f, %f, %f\n", b.x + b.width - a.x,b.y + b.height - a.y, b.z + b.depth - a.z);
        int axis = 0;
        float maxL = xs;
        if(ys > maxL)
        {
            maxL = ys;
            axis = 1;
        }
        if(zs > maxL)
        {
            maxL = zs;
            axis = 2;
        }

        if(axis == 0)
        {
            if(xEntry < 0.0f)
            {
                n.x = 1.f;
            }else
            {
                n.x = -1.0f;
            }
        }else if(axis == 1)
        {
            if(yEntry < 0.0f)
            {
                n.y = 1.f;
            }else
            {
                n.y = -1.0f;
            }
        }else if(axis == 2)
        {
            if(zEntry < 0.0f)
            {
                n.z = 1.f;
            }else
            {
                n.z = -1.0f;
            }
        }
        //printf("coollllllll %f, %f, %f, v: %f, %f, %f. n:%f, %f, %f\n", b.x - a.x, b.y - a.y, b.z - a.z, v.x, v.y, v.z, n.x, n.y, n.z);
        if(maxL > 1.0f)
        {
            n.x = n.y = n.z =0.f;
            return 1.0f; 
        }
    }

    return entry;
}
void CollisionController::removeEntity(fea::EntityId id)
{
    mEntities.erase(id);
}

void CollisionController::checkIfOnGround(fea::EntityPtr entity)
{
    glm::vec3 currentVelocity = entity->getAttribute<glm::vec3>("velocity");
    bool isOnGround = entity->getAttribute<bool>("on_ground");

    if(glm::abs(currentVelocity.y) < 0.6 && !isOnGround)
    {
        entity->setAttribute<bool>("on_ground", true);
        mBus.sendMessage<EntityOnGroundMessage>(EntityOnGroundMessage(entity->getId(), true));
    }
}
