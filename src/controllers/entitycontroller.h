#pragma once
#include <featherkit/entitysystem.h>
#include <featherkit/messaging.h>
#include <map>

class EntityController
{
    public:
        EntityController(fea::MessageBus& b);
        virtual void inspectEntity(fea::WeakEntityPtr entity) = 0;
        virtual void update();
    protected:
        fea::MessageBus& bus;
        std::map<fea::EntityId, fea::WeakEntityPtr> entities;
};
