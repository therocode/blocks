#pragma once
#include "entitycontroller.hpp"
#include "../../application/applicationmessages.hpp"
#include "../entitymessages.hpp"
#include "../../utilities/timer.hpp"

class PhysicsController : 
    public EntityController,
    public fea::MessageReceiver<FrameMessage,
                                GravityRequestedMessage,
                                PhysicsImpulseMessage>
{
    public:
        PhysicsController(fea::MessageBus& bus);
        bool keepEntity(fea::WeakEntityPtr entity) const override;
        void handleMessage(const FrameMessage& received) override;
        void handleMessage(const GravityRequestedMessage& received) override;
        void handleMessage(const PhysicsImpulseMessage& received) override;
    private:
		int accumulator;
		Timer mTimer;
        float gravityConstant;
};
