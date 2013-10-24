#include "collisioncontroller.h"
#include "../../world/worldinterface.h"

CollisionController::CollisionController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
    bus.addMessageSubscriber<EntityMoveRequestedMessage>(*this);
}

void CollisionController::inspectEntity(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->hasAttribute("position") &&
       locked->hasAttribute("velocity") &&
       locked->hasAttribute("hitbox"))
    {
        mEntities.emplace(locked->getId(), entity);
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
    
	 //glm::vec3 velocity = mEntities.at(id).lock()->getAttribute<glm::vec3>("velocity");
    //    velocity.y *= -0.5f;
     //   mEntities.at(id).lock()->setAttribute<glm::vec3>("velocity", velocity);
	
	if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(0.f, -0.5f, 0.f)) != 0)
    {
		move.y = (glm::floor(approvedPosition.y) + 0.5f) - approvedPosition.y ;
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
	
	
	if(move.y < 999)
		approvedPosition.y += move.y;
	if(move.x < 999)
		approvedPosition.x += move.x;
	if(move.z < 999)
		approvedPosition.z += move.z;
	

    mEntities.at(id).lock()->setAttribute<glm::vec3>("position", approvedPosition);
    
    mBus.sendMessage<EntityMovedMessage>(EntityMovedMessage(id, requestedPosition, approvedPosition));
}

void CollisionController::removeEntity(fea::EntityId id)
{
    mEntities.erase(id);
}
