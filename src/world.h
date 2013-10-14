#pragma once
#include "blockstd.h"
#include "dimension.h"
#include "worldinterface.h"
#include "entitysystem.h"
#include <featherkit/messaging.h>
#include "messages.h"

class World : 
        public fea::MessageReceiver<CameraUpdatedMessage>
{
    public:
        World(fea::MessageBus& messageBus);
		~World();
        void initialise();
        void update();
        virtual void handleMessage(const CameraUpdatedMessage& received);
    private:
		glm::vec3 mCamPos, mCamDir;
        fea::MessageBus& bus;
        Dimension standardDimension;
        EntitySystem entitySystem;
        WorldInterface worldInterface;
};
