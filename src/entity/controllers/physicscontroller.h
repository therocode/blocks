#pragma once
#include "entitycontroller.h"
#include "../../messages.h"

class PhysicsController : 
    public EntityController,
    public fea::MessageReceiver<GravityRequestedMessage>
{
    public:
        PhysicsController(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~PhysicsController();
        virtual void inspectEntity(fea::WeakEntityPtr entity);
        virtual void update();
        void handleMessage(const GravityRequestedMessage& received);
        virtual void removeEntity(fea::EntityId id);
    private:
        float gravityConstant;
};
