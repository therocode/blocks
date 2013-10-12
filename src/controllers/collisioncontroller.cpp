#include "collisioncontroller.h"

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

    if(approvedPosition.y < 0.0f)
    {
        approvedPosition.y = 0.0f;

        glm::vec3 velocity = mEntities.at(id).lock()->getAttribute<glm::vec3>("velocity");
        velocity.y *= -1.0f;
        mEntities.at(id).lock()->setAttribute<glm::vec3>("velocity", velocity);
    }

    mEntities.at(id).lock()->setAttribute<glm::vec3>("position", approvedPosition);
    
    mBus.sendMessage<EntityMovedMessage>(EntityMovedMessage(id, requestedPosition, approvedPosition));
}
