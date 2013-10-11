#include "physicscontroller.h"

void PhysicsController::inspectEntity(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->hasAttribute("mass") && 
       locked->hasAttribute("velocity") &&
       locked->hasAttribute("acceleration") && 
       locked->hasAttribute("boundingbox"))
    {
        entities.add(entity);
    }
}

void PhysicsController::update()
{

}
