#pragma once
#include <featherkit/messaging.h>
#include <featherkit/entitysystem.h>
#include <map>

class WorldInterface;

class EntityController
{
    public:
        EntityController(fea::MessageBus& bus, WorldInterface& worldInterface);
        virtual ~EntityController(){};
        virtual void inspectEntity(fea::WeakEntityPtr entity) = 0;
        virtual void update();
        virtual void removeEntity(fea::EntityId id) = 0;
    protected:
        fea::MessageBus& mBus;
        WorldInterface& mWorldInterface;
        std::map<fea::EntityId, fea::WeakEntityPtr> mEntities;
};
