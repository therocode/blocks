#pragma once
#include <fea/util.hpp>
#include "../../gameinterface.hpp"
#include "entitycontroller.hpp"
#include "../../application/applicationmessages.hpp"
#include "../entitymessages.hpp"

class MovementController : 
    public EntityController,
    public fea::MessageReceiver<FrameMessage, 
                                EntityJumpMessage>
{
    public:
        MovementController(fea::MessageBus& bus);
        bool keepEntity(fea::WeakEntityPtr entity) const override;
        void handleMessage(const FrameMessage& received) override;
        void handleMessage(const EntityJumpMessage& received) override;
};
