#pragma once
#include "entitycontroller.h"
#include "../messages.h"

class CollisionController : public EntityController,
                            public fea::MessageReceiver<EntityMoveRequestedMessage>
{
    public:
        CollisionController(fea::MessageBus& b);
        virtual void inspectEntity(fea::WeakEntityPtr entity) override;
        void handleMessage(const EntityMoveRequestedMessage& message) override;
    private:
};