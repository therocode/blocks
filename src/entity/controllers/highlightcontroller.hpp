#pragma once
#include "entitycontroller.hpp"
#include "../entitymessages.hpp"

class HighlightController : 
    public EntityController,
    public fea::MessageReceiver<EntityMovedMessage,
                                EntityEnteredWorldMessage>
{
    public:
        HighlightController(fea::MessageBus& bus, GameInterface& gameInterface);
        virtual void inspectEntity(fea::WeakEntityPtr entity) override;
        virtual void removeEntity(fea::EntityId id) override;
        void handleMessage(const EntityMovedMessage& received);
        void handleMessage(const EntityEnteredWorldMessage& received);
    private:
};
