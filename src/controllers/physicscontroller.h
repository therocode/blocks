#pragma once
#include "entitycontroller.h"

class PhysicsController : public EntityController
{
    public:
        PhysicsController(fea::MessageBus& b);
        virtual void inspectEntity(fea::WeakEntityPtr entity);
        virtual void update();
};
