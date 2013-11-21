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
			}else{
				entity->setAttribute<glm::vec3>("velocity", velocity * 0.9f);
			}
        }else{
			if(AABBOnGround(a) && velocity.y <= 0.001f){
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
    glm::vec3 v = approvedPosition - oldPosition;
	
	glm::vec3 ignoreAxis = glm::vec3(0);
	float n = 0.0;
	int steps = 0;
	
	while(n < 1.0f){
		steps++;
		glm::vec3 normal = glm::vec3(0);
		a.x = oldPosition.x - size.x * 0.5f;
		a.y = oldPosition.y - size.y * 0.5f;
		a.z = oldPosition.z - size.z * 0.5f;
		n = sweepAroundAABB(a, v, normal, ignoreAxis);
		// Renderer::sDebugRenderer.drawBox(a.x + a.width*0.5f, a.y + a.height*0.5f, a.z + a.depth*0.5f, a.width  + 0.001f, a.height + 0.001f, a.depth + 0.001f, DebugRenderer::ORANGE);
		if(n < 1.f)
		{ 
			for(int i = 0; i < 3; i++){
				if(normal[i] != 0){
					ignoreAxis[i] = 1.0;
					break;
				}
			}
			approvedPosition = oldPosition + v * glm::max(n - 0.08f, 0.0f);
			oldPosition = approvedPosition;
			
			velocity = mEntities.at(id).lock()->getAttribute<glm::vec3>("velocity");
			glm::vec3 acceleration = mEntities.at(id).lock()->getAttribute<glm::vec3>("acceleration");

			float remainingTime = 1.0f - n;
			float magn = glm::length(velocity) * remainingTime;
			float collDot = glm::dot(normal, v);
			collDot = glm::clamp(collDot, -1.f, 1.f);
			glm::vec3 c = (glm::vec3(1.f) - glm::abs(normal)) * v;

			glm::vec3 additionalVelocity = c * remainingTime;
			approvedPosition += additionalVelocity;
			
			//velocity += additionalVelocity;
			//velocity is either 0 or 1, depending if it collided or not. when all is working it should use the normal to do stuff.
			//printf("noral: %f, %f, %f\n", normal.x, normal.y, normal.z);
			velocity     *= glm::vec3(1.0) - glm::abs(normal);
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

float CollisionController::sweepAroundAABB(const AABB a, glm::vec3 velocity, glm::vec3& outNormal, const glm::vec3 ignoreAxis)
{
	AABB b;
	int sx = 1, sy = 2, sz = 1;
    float n = 1.0f;
    glm::vec3 normal = glm::vec3(0.f);
	float longest = 99999.f;
    //Loop througha cube of blocks and check if they are passableor not
    for(float x = -sx; x <= sx; x++)
    {
        for(float y = -sy; y <= sy + 1; y++)
        {
            for(float z = -sz; z <= sz; z++)
            {
                glm::vec3 cubePos = glm::vec3(a.x + velocity.x, a.y + velocity.y, a.z + velocity.z) + glm::vec3(x, y, z);
				//set position of aabb B
				b.x = glm::floor(cubePos.x);                 
				b.y = glm::floor(cubePos.y);
				b.z = glm::floor(cubePos.z);
			
				//if(b.x < 0) b.x --;
				//if(b.y < 0) b.y --;
				//if(b.z < 0) b.z --;	
				
				//b.x = (int)b.x;
				//b.y = (int)b.y;
				//b.z = (int)b.z;
				if(mWorldInterface.getVoxelType(cubePos) != 0)
                {
				    glm::vec3 norm;
					// renderDebugAABB(b, DebugRenderer::GREEN);
                   

                    //A is the entity, B is block in world, v is newPosition - oldPosition. Function should set norm to a normal on which face it collided. returns depth, which is between 0 and 1.
                    float nn = sweepAABB(a, b, velocity, norm);
                 
                    //If depth is shallower than before, set the new depth to the new value.
					int axis = 0;
					for(int i = 0; i < 3; i++)
					{
						if(norm[i] != 0){
							axis = i;
							break;
						}
					}
                    if(nn < n ){
						// glm::vec3 aa = glm::vec3(a.x + a.width * 0.5f, a.y + a.height * 0.5f, a.z + a.depth * 0.5f);
						// glm::vec3 bb = glm::vec3(b.x + b.width * 0.5f, b.y + b.height * 0.5f, b.z + b.depth * 0.5f);
						// float l2 = glm::length2(aa - bb);
						// if(l2 < longest){
							// longest = l2;
							n = nn;
							normal = norm;
							// Renderer::sDebugRenderer.drawBox(b.x + b.width*0.5f, b.y + b.height*0.5f, b.z + b.depth*0.5f, b.width  + 0.01f, b.height + 0.01f, b.depth + 0.01f, DebugRenderer::GREEN);
						// }
					}
                }//else
					//renderDebugAABB(b, DebugRenderer::RED);
            }
        }
    }
	if(n < 1.0)
	{
		outNormal = normal;
		return n;
	}
	return 1.0;
}
void CollisionController::renderDebugAABB(const AABB a, const int color)
{
	Renderer::sDebugRenderer.drawBox(a.x + a.width*0.5f, a.y + a.height*0.5f, a.z + a.depth*0.5f, a.width  + 0.01f, a.height + 0.01f, a.depth + 0.01f, color);
}
bool CollisionController::AABBOnGround(AABB a)
{
	float s = 0.05f;
	AABB b;
	a.x += a.width * s;
	a.z += a.depth * s;
	a.width *= 1.0f - s*2.0f;
	a.depth *= 1.0f - s*2.0f;
	a.y -= 0.01f;
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
			b.x = pos.x;                 
			b.y = pos.y;
			b.z = pos.z;
			
			if(b.x < 0) b.x --;
			if(b.y < 0) b.y --;
			if(b.z < 0) b.z --;
			
			b.x = (int)b.x;
			b.y = (int)b.y;
			b.z = (int)b.z;
			if(AABBAABB(a, b)){
				return true;
			}
		}
	}
	return false;
	
}
bool CollisionController::AABBAABB(const AABB a, const AABB b) const
{
	glm::vec3 aSize = glm::vec3(a.width * 0.5f, a.height * 0.5f, a.depth * 0.5f);
	glm::vec3 bSize = glm::vec3(b.width * 0.5f, b.height * 0.5f, b.depth * 0.5f);
	glm::vec3 v = glm::abs(glm::vec3(b.x + bSize.x, b.y + bSize.y, b.z + bSize.z) - glm::vec3(a.x + aSize.x, a.y + aSize.y, a.z + aSize.z));
	return (v.x < (aSize.x + bSize.x)) && (v.y < aSize.y + bSize.y) && (v.z < aSize.z + bSize.z);
    //if(a.x <= b.x + b.width && a.y <= b.y + b.height && a.z <= b.z + b.depth)
      ///////7  if(a.x + a.width >= b.x && a.y + a.depth >= b.y && a.z + a.depth >= b.z)
           // return true;
    //return false;
}
float CollisionController::sweepAABB(const AABB a, const AABB b, const glm::vec3 v, glm::vec3& n)
{
    AABB newAABB = a;
    newAABB.x += v.x;
    newAABB.y += v.y;
    newAABB.z += v.z;
	n.x = n.y = n.z = 0;
    if(!AABBAABB(newAABB, b))
    {
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
	
    if(v.x == 0)
    {
        xs = -infinity;
        xe =  infinity;
    }else
    {
        xs = xEntry / v.x;
        xe = xExit  / v.x;
    }

    if(v.y == 0)
    {
        ys = -infinity;
        ye =  infinity;
    }else
    {
        ys = yEntry / v.y;
        ye = yExit  / v.y;
    }
    if(v.z  == 0)
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
    }else if(xs > 1.0f && ys > 1.0f && zs > 1.0f)
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
//Renderer::sDebugRenderer.drawBox(b.x + b.width*0.5f, b.y + b.height*0.5f, b.z + b.depth*0.5f, b.width  + 0.001f, b.height + 0.001f, b.depth + 0.001f, DebugRenderer::GREEN);
     
    return entry;
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
