#pragma once
#include "../blockstd.h"
#include "world.h"
#include "worldinterface.h"
#include "../entity/entitysystem.h"
#include <featherkit/messaging.h>
#include "../rendering/renderingmessages.h"
#include "worldmessages.h"

class Universe : 
        public fea::MessageReceiver<SetVoxelMessage>
{
    public:
        Universe(fea::MessageBus& messageBus);
		~Universe();
        void setup();
        void update();
        void destroy();
        virtual void handleMessage(const SetVoxelMessage& received);
        WorldInterface& getWorldInterface();
    private:
		glm::vec3 mCamPos, mCamDir;
        fea::MessageBus& bus;
        World standardWorld;
        EntitySystem entitySystem;
        WorldInterface worldInterface;
};
