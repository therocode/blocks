#pragma once
#include "dimension.h"
#include "worldinterface.h"
#include "entitysystem.h"
#include <featherkit/messaging.h>

class World
{
    public:
        World(fea::MessageBus& messageBus);
        void initialise();
        void update();
    private:
        fea::MessageBus& bus;
        Dimension standardDimension;
        EntitySystem entitySystem;
        WorldInterface worldInterface;
};
