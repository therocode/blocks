#pragma once
#include "entitycontroller.hpp"
#include "../entitymessages.hpp"

class HighlightController : 
    public EntityController,
    public fea::MessageReceiver<EntityMovedMessage,
                                EntityEnteredWorldMessage>
{
    public:
        HighlightController(fea::MessageBus& bus);
        bool keepEntity(fea::WeakEntityPtr entity) const override;
        void entityKept(fea::WeakEntityPtr entity) override;
        void entityDropped(fea::WeakEntityPtr entity) override;
        void handleMessage(const EntityMovedMessage& received);
        void handleMessage(const EntityEnteredWorldMessage& received);
    private:
};
