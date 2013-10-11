#include "physicscontroller.h"

virtual void PhysicsController::inspectEntity(fea::WeakEntityPtr entity)
{
    EntityPtr locked = entity.lock();

    if(locked->hasAttribute("mass") && 
       locked->hasAttribute("velocity") &&
       locked->hasAttribute("acceleration") && 
       locked->hasAttribute("boundingbox")
    {
        entities.add(entity);
    }
}
