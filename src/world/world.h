#pragma once
#include "../blockstd.h"
#include "dimension.h"
#include "worldinterface.h"
#include "../entity/entitysystem.h"
#include <featherkit/messaging.h>
#include "../rendering/renderingmessages.h"
#include "worldmessages.h"

class World : 
        public fea::MessageReceiver<PlayerEntersChunkMessage>,
        public fea::MessageReceiver<SetVoxelMessage>
{
    public:
        World(fea::MessageBus& messageBus);
		~World();
        void initialise();
        void update();
        virtual void handleMessage(const PlayerEntersChunkMessage& received);
        virtual void handleMessage(const SetVoxelMessage& received);
        WorldInterface& getWorldInterface();
    private:
		glm::vec3 mCamPos, mCamDir;
        fea::MessageBus& bus;
        Dimension standardDimension;
        EntitySystem entitySystem;
        WorldInterface worldInterface;
};
