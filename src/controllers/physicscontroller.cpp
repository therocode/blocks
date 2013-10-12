#include "physicscontroller.h"
#include "../blockstd.h"
#include "../messages.h"

PhysicsController::PhysicsController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
}

void PhysicsController::inspectEntity(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->hasAttribute("mass") && 
       locked->hasAttribute("position") &&
       locked->hasAttribute("velocity") &&
       locked->hasAttribute("acceleration") && 
       locked->hasAttribute("hitbox"))
    {
        mEntities.emplace(locked->getId(), entity);
    }
}

void PhysicsController::update()
{
    for(auto wEntity : mEntities)
    {
        fea::EntityPtr entity = wEntity.second.lock();

        glm::vec3 currentPosition = entity->getAttribute<glm::vec3>("position");
        glm::vec3 currentVelocity = entity->getAttribute<glm::vec3>("velocity");
        glm::vec3 acceleration = entity->getAttribute<glm::vec3>("acceleration") + glm::vec3(0.0f, -0.001f, 0.0f);
        float mass = entity->getAttribute<float>("mass");

        glm::vec3 newVelocity = currentVelocity + acceleration;
        glm::vec3 newPosition = currentPosition + newVelocity;

        entity->setAttribute<glm::vec3>("velocity", newVelocity);

        mBus.sendMessage<EntityMoveRequestedMessage>(EntityMoveRequestedMessage(entity->getId(), newPosition));
    }
}
