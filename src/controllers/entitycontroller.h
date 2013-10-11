#pragma once
#include <featherkit/entitysystem.h>

class EntityController
{
    public:
        virtual void inspectEntity(fea::WeakEntityPtr entity) = 0;
        virtual void update() = 0;
    protected:
        fea::EntityGroup entities;
};
