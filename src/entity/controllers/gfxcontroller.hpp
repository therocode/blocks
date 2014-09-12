#pragma once
#include "entitycontroller.hpp"
#include "../entitymessages.hpp"

class GfxController : public EntityController,
                      public fea::MessageReceiver<EntityMovedMessage>
{
    public:
        GfxController(fea::MessageBus& bus, WorldInterface& worldinterface);
        ~GfxController();
        virtual void inspectEntity(fea::WeakEntityPtr entity) override;
        void handleMessage(const EntityMovedMessage& message) override;
        virtual void removeEntity(fea::EntityId id) override;
};