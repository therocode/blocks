#include "collisioncontroller.h"
#include "../../world/worldinterface.h"
#include <iostream>

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

	int sx = 3, sy = 3, sz = 3;
	float n = 1.0f;
	glm::vec3 normal = glm::vec3(0.f);
	for(int x = -sx; x <= sx; x++)
	{
		for(int y = -sy; y <= sy; y++)
		{
			for(int z = -sz; z <= sz; z++)
			{
				glm::vec3 cubePos = glm::floor(oldPosition) + glm::vec3(x, y, z);
				if(mWorldInterface.getVoxelType(cubePos) != 0)
				{
					
					glm::vec3 norm;
					// v.x = 0; 
					// v.y = -100; v.z = 0;
					b.x = cubePos.x;
					b.y = cubePos.y;
					b.z = cubePos.z;
					float nn = sweepAABB(a, b, v, norm);
					 // printf("nn:%f\n", nn);
					n = glm::min(nn, n);
					if(n != nn)normal = norm;
					
				}
			}
		}
	}
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
	
	// velocity *= (n !=  1.0) ? 0: 1;
	
    entity->setAttribute<glm::vec3>("velocity", velocity);
    entity->setAttribute<glm::vec3>("position", approvedPosition);
    
    mBus.sendMessage<EntityMovedMessage>(EntityMovedMessage(id, requestedPosition, approvedPosition));

    // if(entity->getAttribute<bool>("on_ground"))
    // {
        // if(approvedPosition.y < oldPosition.y)
            // approvedPosition.y = oldPosition.y;
    // }
	
	// if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(0.f, -0.5f, 0.f)) != 0)
    // {
		// move.y = (glm::floor(approvedPosition.y) + 0.5f) - approvedPosition.y;
        
        // checkIfOnGround(entity);
    // }
	
	// if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(0.f, 0.5f, 0.f)) != 0)
    // {
		// float p = (glm::ceil(approvedPosition.y) - 0.5f) - approvedPosition.y;
		// if(glm::abs(move.y) > glm::abs(p))
		// {
			// move.y = p;
		// }
    // }
	
	// if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(-0.5f, 0.f, 0.f)) != 0)
    // {
		// move.x = (glm::floor(approvedPosition.x) + 0.5f) - approvedPosition.x;
    // }
	
	// if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(0.5f, 0.0f, 0.f)) != 0)
    // {
		// float p = (glm::ceil(approvedPosition.x) - 0.5f) - approvedPosition.x;
		// if(glm::abs(move.x) > glm::abs(p))
		// {
			// move.x = p;
		// }
    // }
	
	// if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(0,0,-0.5f)) != 0)
    // {
		// move.z = (glm::floor(approvedPosition.z) + 0.5f) - approvedPosition.z;
    // }
	
	// if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(0,0,0.5f)) != 0)
    // {
		// float p = (glm::ceil(approvedPosition.z) - 0.5f) - approvedPosition.z;
		// if(glm::abs(move.z) > glm::abs(p))
		// {
			// move.z = p;
		// }
    // }
	
	

   
	// if(move.y < 999)
	// {
		// approvedPosition.y += move.y;
		// velocity.y = 0;//*= -0.5f;
	// }
	// if(move.x < 999)
	// {
		// approvedPosition.x += move.x;
		// velocity.x = 0.f;
	// }
	// if(move.z < 999)
	// {
		// approvedPosition.z += move.z;
		// velocity.z = 0.f;
	// }

}
float CollisionController::sweepAABB(const AABB a, const AABB b, const glm::vec3 v, glm::vec3& n)
{
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
	
	float xs, ys, zs;
	float xe, ye, ze;
	float infinity = std::numeric_limits<float>::infinity();
	
	if(v.x == 0.0f)
	{
		xs = -infinity;
		xe =  infinity;
	}else
	{
		xs = xEntry / v.x;
		xe = xExit  / v.x;
	}
	
	if(v.y == 0.0f)
	{
		ys = -infinity;
		ye =  infinity;
	}else
	{
		ys = yEntry / v.y;
		ye = yExit  / v.y;
	}
	
	if(v.z == 0.0f)
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
	// printf("entries: %f, %f, %f\n", xs, ys, zs);
	if(entry > exit || xs < 0.f && ys < 0 && zs < 0 || xe > 1.0f && ye > 1.0f && ze > 1.0f)
	{
		n.x = n.y = n.z = 0.0f;
		return 1.f;
	}else
	{
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
   
    if(fabs(currentVelocity.y) < 0.06 && !isOnGround)
    {
        entity->setAttribute<bool>("on_ground", true);
        mBus.sendMessage<EntityOnGroundMessage>(EntityOnGroundMessage(entity->getId(), true));
    }
}
