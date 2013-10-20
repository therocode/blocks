#pragma once
#include "entitycontroller.h"
#include "../../application/applicationmessages.h"

//message bus,
//takes inputaction messages
//deals with them
//create camera instruction message --> go to the renderer?

class PlayerController : 
    public EntityController,
    public fea::MessageReceiver<PlayerJoinedMessage>
{
    public:
        PlayerController(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~PlayerController();
        virtual void inspectEntity(fea::WeakEntityPtr entity);
        virtual void removeEntity(fea::EntityId id);
        void handleMessage(const PlayerJoinedMessage& received);
    private:
        std::unordered_map<size_t, fea::WeakEntityPtr> mPlayerEntities;
};
