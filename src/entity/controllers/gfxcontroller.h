#pragma once
#include "entitycontroller.h"
#include "../entitymessages.h"

class GfxController : public EntityController,
                      public fea::MessageReceiver<EntityMovedMessage>
{
    public:
        GfxController(fea::MessageBus& bus, WorldInterface& worldinterface);
        virtual void inspectEntity(fea::WeakEntityPtr entity) override;
        void handleMessage(const EntityMovedMessage& message) override;
        virtual void removeEntity(fea::EntityId id) override;
};
