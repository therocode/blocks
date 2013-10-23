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

    if(mWorldInterface.getVoxelType(approvedPosition + glm::vec3(0.f, -0.5f, 0.f)) != 0)
    {
        glm::vec3 velocity = mEntities.at(id).lock()->getAttribute<glm::vec3>("velocity");
        velocity.y *= -0.5f;
        mEntities.at(id).lock()->setAttribute<glm::vec3>("velocity", velocity);
		approvedPosition.y = glm::floor(approvedPosition.y) + 0.5f;
    }

    mEntities.at(id).lock()->setAttribute<glm::vec3>("position", approvedPosition);
    
    mBus.sendMessage<EntityMovedMessage>(EntityMovedMessage(id, requestedPosition, approvedPosition));
}

void CollisionController::removeEntity(fea::EntityId id)
{
    mEntities.erase(id);
}
