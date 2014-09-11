#pragma once
#include <fea/util.hpp>
#include <fea/entitysystem.hpp>
#include <map>

class WorldInterface;

class EntityController
{
    public:
        EntityController(fea::MessageBus& bus, WorldInterface& worldInterface);
        virtual ~EntityController(){};
        virtual void inspectEntity(fea::WeakEntityPtr entity) = 0;
        virtual void onFrame(int dt);
        virtual void removeEntity(fea::EntityId id) = 0;
    protected:
        fea::MessageBus& mBus;
        WorldInterface& mWorldInterface;
        std::map<fea::EntityId, fea::WeakEntityPtr> mEntities;
};
