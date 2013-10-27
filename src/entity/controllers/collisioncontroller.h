#pragma once
#include "entitycontroller.h"
#include "../entitymessages.h"

class CollisionController : public EntityController,
                            public fea::MessageReceiver<EntityMoveRequestedMessage>
{
    public:
        CollisionController(fea::MessageBus& bus, WorldInterface& worldInterface);
        virtual void inspectEntity(fea::WeakEntityPtr entity) override;
        void onFrame() override;
        void handleMessage(const EntityMoveRequestedMessage& message) override;
        virtual void removeEntity(fea::EntityId id);
    private:
        void checkIfOnGround(fea::EntityPtr entity);
};
