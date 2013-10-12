#pragma once
#include "entitycontroller.h"

class PhysicsController : public EntityController
{
    public:
        PhysicsController(fea::MessageBus& bus, WorldInterface& worldInterface);
        virtual void inspectEntity(fea::WeakEntityPtr entity);
        virtual void update();
};
