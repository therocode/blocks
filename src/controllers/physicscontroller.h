#pragma once
#include "entitycontroller"

class PhysicsController : public EntityController
{
    public:
        virtual void inspectEntity(fea::WeakEntityPtr entity);
};
