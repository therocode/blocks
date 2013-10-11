#pragma once
#include <featherkit/entitysystem.h>
#include <map>

class EntityController
{
    public:
        virtual void inspectEntity(fea::WeakEntityPtr entity) = 0;
        virtual void update() = 0;
    protected:
        std::map<fea::EntityId, fea::WeakEntityPtr> entities;
};
