#include "collisioncontroller.h"
#include <iostream>

CollisionController::CollisionController(fea::MessageBus& b) : EntityController(b)
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
        entities.emplace(locked->getId(), entity);
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

        glm::vec3 velocity = entities.at(id).lock()->getAttribute<glm::vec3>("velocity");
        velocity.y *= -1.0f;
        entities.at(id).lock()->setAttribute<glm::vec3>("velocity", velocity);
    }

    entities.at(id).lock()->setAttribute<glm::vec3>("position", approvedPosition);
    
    bus.sendMessage<EntityMovedMessage>(EntityMovedMessage(id, requestedPosition, approvedPosition));

    std::cout << "entity moved to " << approvedPosition.x << " " << approvedPosition.y << " " << approvedPosition.z << "\n";
}
