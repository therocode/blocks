#pragma once
#include "chunk.h"
#include "landscape.h"
#include <featherkit/messaging.h>

class Dimension
{
    public:
        Dimension(fea::MessageBus& messageBus);
        void initialise(); 
    private:
        fea::MessageBus& bus;
        Landscape landscape;
};
