#pragma once
#include "entitycontroller.h"
#include "../messages.h"

class GfxController : public EntityController,
                      public fea::MessageReceiver<EntityMovedMessage>
{
    public:
        GfxController(fea::MessageBus& b);
        virtual void inspectEntity(fea::WeakEntityPtr entity) override;
        void handleMessage(const EntityMovedMessage& message) override;
};
