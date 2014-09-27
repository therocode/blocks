#pragma once
#include <fea/util.hpp>
#include "../../gameinterface.hpp"
#include "entitycontroller.hpp"
#include "../entitymessages.hpp"

class MovementController : 
    public EntityController,
    public fea::MessageReceiver<EntityJumpMessage>
{
    public:
        MovementController(fea::MessageBus& bus, GameInterface& worldInterface);
        virtual void inspectEntity(fea::WeakEntityPtr entity) override;
        virtual void removeEntity(fea::EntityId id) override;
        virtual void onFrame(int dt) override;
        void handleMessage(const EntityJumpMessage& received);
};
