#include "physicscontroller.h"

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

}
