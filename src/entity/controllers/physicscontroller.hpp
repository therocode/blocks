#pragma once
#include "entitycontroller.hpp"
#include "../entitymessages.hpp"
#include "../../utilities/timer.hpp"

class PhysicsController : 
    public EntityController,
    public fea::MessageReceiver<GravityRequestedMessage>,
    public fea::MessageReceiver<PhysicsImpulseMessage>
{
    public:
        PhysicsController(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~PhysicsController();
        virtual void inspectEntity(fea::WeakEntityPtr entity);
        virtual void onFrame(int dt) override;
        void handleMessage(const GravityRequestedMessage& received);
        void handleMessage(const PhysicsImpulseMessage& received);
        virtual void removeEntity(fea::EntityId id);
    private:
		int accumulator;
		Timer mTimer;
        float gravityConstant;
};
