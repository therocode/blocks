#pragma once
#include <fea/util.hpp>
#include "../../world/worldinterface.hpp"
#include "entitycontroller.hpp"

class MovementController : 
    public EntityController,
    public fea::MessageReceiver<EntityJumpMessage>
{
    public:
        MovementController(fea::MessageBus& bus, WorldInterface& worldInterface);
        virtual void inspectEntity(fea::WeakEntityPtr entity) override;
        virtual void removeEntity(fea::EntityId id) override;
        virtual void onFrame(int dt) override;
        void handleMessage(const EntityJumpMessage& received);
};
