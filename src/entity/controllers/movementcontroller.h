#pragma once
#include <featherkit/messaging.h>
#include "../../world/worldinterface.h"

class MovementController : 
    public EntityController
{
    public:
        MovementController(fea::MessageBus& bus, WorldInterface& worldInterface);
        virtual void inspectEntity(fea::WeakEntityPtr entity) override;
        virtual void removeEntity(fea::EntityId id) override;
        virtual void onFrame() override;
};
