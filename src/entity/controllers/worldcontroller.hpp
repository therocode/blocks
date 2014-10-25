#pragma once
#include "entitycontroller.hpp"
#include "../entitymessages.hpp"

class WorldController : 
    public EntityController,
    public fea::MessageReceiver<EntityWorldTransferRequestedMessage>
{
    public:
        WorldController(fea::MessageBus& bus);
        void handleMessage(const EntityWorldTransferRequestedMessage& received);
    private:
};
