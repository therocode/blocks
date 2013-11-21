#pragma once
#include <featherkit/messaging.h>
#include "../../world/worldinterface.h"

class MovementController : 
    public EntityController,
    public fea::MessageReceiver<EntityJumpMessage>
{
    public:
        ~MovementController();
        MovementController(fea::MessageBus& bus, WorldInterface& worldInterface);
        virtual void inspectEntity(fea::WeakEntityPtr entity) override;
        virtual void removeEntity(fea::EntityId id) override;
        virtual void onFrame(int dt) override;
        void handleMessage(const EntityJumpMessage& received);
};
