#include "physicscontroller.h"
#include "../blockstd.h"
#include <iostream>

void PhysicsController::inspectEntity(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->hasAttribute("mass") && 
       locked->hasAttribute("position") &&
       locked->hasAttribute("velocity") &&
       locked->hasAttribute("acceleration") && 
       locked->hasAttribute("hitbox"))
    {
        entities.emplace(locked->getId(), entity);
    }
}

void PhysicsController::update()
{
    for(auto wEntity : entities)
    {
        fea::EntityPtr entity = wEntity.second.lock();

        glm::vec3 currentPosition = entity->getAttribute<glm::vec3>("position");
        glm::vec3 currentVelocity = entity->getAttribute<glm::vec3>("velocity");
        glm::vec3 acceleration = entity->getAttribute<glm::vec3>("acceleration");
        float mass = entity->getAttribute<float>("mass");

        glm::vec3 newVelocity = currentVelocity + acceleration;
        glm::vec3 newPosition = currentPosition + newVelocity;

        entity->setAttribute<glm::vec3>("position", newPosition);
        entity->setAttribute<glm::vec3>("velocity", newVelocity);

        std::cout << "now the position of the human is " << newPosition.x << " " << newPosition.y << " " << newPosition.z << "\n";
    }
}
