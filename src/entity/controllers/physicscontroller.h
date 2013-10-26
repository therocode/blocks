#pragma once
#include "entitycontroller.h"
#include "../entitymessages.h"
#include "../../utilities/timer.h"

class PhysicsController : 
    public EntityController,
    public fea::MessageReceiver<GravityRequestedMessage>,
    public fea::MessageReceiver<PhysicsImpulseMessage>
{
    public:
        PhysicsController(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~PhysicsController();
        virtual void inspectEntity(fea::WeakEntityPtr entity);
        virtual void onFrame() override;
        void handleMessage(const GravityRequestedMessage& received);
        void handleMessage(const PhysicsImpulseMessage& received);
        virtual void removeEntity(fea::EntityId id);
    private:
		int accumulator;
		Timer mTimer;
        float gravityConstant;
};
