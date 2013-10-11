#pragma once
#include "entitycontroller.h"

class PhysicsController : public EntityController
{
    public:
        virtual void inspectEntity(fea::WeakEntityPtr entity);
        virtual void update();
};
