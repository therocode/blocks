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
                entity->setAttribute<glm::vec3>("velocity", entity->getAttribute<glm::vec3>("velocity") * 0.7f);
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

    if(entity->getAttribute<bool>("on_ground"))
    {
        if(approvedPosition.y < oldPosition.y)
            approvedPosition.y = oldPosition.y;
    }
	
	if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(0.f, -0.5f, 0.f)) != 0)
    {
		move.y = (glm::floor(approvedPosition.y) + 0.5f) - approvedPosition.y;
        
        checkIfOnGround(entity);
    }
	
	if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(0.f, 0.5f, 0.f)) != 0)
    {
		float p = (glm::ceil(approvedPosition.y) - 0.5f) - approvedPosition.y;
		if(glm::abs(move.y) > glm::abs(p))
		{
			move.y = p;
		}
    }
	
	if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(-0.5f, 0.f, 0.f)) != 0)
    {
		move.x = (glm::floor(approvedPosition.x) + 0.5f) - approvedPosition.x;
    }
	
	if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(0.5f, 0.0f, 0.f)) != 0)
    {
		float p = (glm::ceil(approvedPosition.x) - 0.5f) - approvedPosition.x;
		if(glm::abs(move.x) > glm::abs(p))
		{
			move.x = p;
		}
    }
	
	if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(0,0,-0.5f)) != 0)
    {
		move.z = (glm::floor(approvedPosition.z) + 0.5f) - approvedPosition.z;
    }
	
	if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(0,0,0.5f)) != 0)
    {
		float p = (glm::ceil(approvedPosition.z) - 0.5f) - approvedPosition.z;
		if(glm::abs(move.z) > glm::abs(p))
		{
			move.z = p;
		}
    }
	
	
	glm::vec3 velocity = mEntities.at(id).lock()->getAttribute<glm::vec3>("velocity");
    
   
	if(move.y < 999)
	{
		approvedPosition.y += move.y;
		velocity.y = 0;//*= -0.5f;
	}
	if(move.x < 999)
	{
		approvedPosition.x += move.x;
		velocity.x *= -0.5f;
	}
	if(move.z < 999)
	{
		approvedPosition.z += move.z;
		velocity.z *= -0.5f;
	}
	entity->setAttribute<glm::vec3>("velocity", velocity);
    entity->setAttribute<glm::vec3>("position", approvedPosition);
    
    mBus.sendMessage<EntityMovedMessage>(EntityMovedMessage(id, requestedPosition, approvedPosition));
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
