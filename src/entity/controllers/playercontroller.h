#pragma once
#include "entitycontroller.h"
#include "../../application/applicationmessages.h"
#include "../../input/inputmessages.h"

class PlayerController : 
    public EntityController,
    public fea::MessageReceiver<PlayerJoinedMessage>,
    public fea::MessageReceiver<PlayerActionMessage>
{
    public:
        PlayerController(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~PlayerController();
        virtual void inspectEntity(fea::WeakEntityPtr entity);
        virtual void removeEntity(fea::EntityId id);
        void handleMessage(const PlayerJoinedMessage& received);
        void handleMessage(const PlayerActionMessage& received);
    private:
        std::unordered_map<fea::EntityId, fea::WeakEntityPtr> mPlayerEntities;
};
