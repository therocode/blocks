#pragma once
#include "entitycontroller.hpp"
#include "../entitymessages.hpp"

class GfxController : public EntityController,
                      public fea::MessageReceiver<EntityMovedMessage>
{
    public:
        GfxController(fea::MessageBus& bus);
        bool keepEntity(fea::WeakEntityPtr entity) const override;
        void entityKept(fea::WeakEntityPtr entity) override;
        void handleMessage(const EntityMovedMessage& message) override;
        void entityDropped(fea::WeakEntityPtr entity) override;
};
