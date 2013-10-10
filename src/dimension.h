#pragma once
#include "chunk.h"
#include "landscape.h"
#include "focuspoint.h"
#include <featherkit/messaging.h>

class Dimension
{
    public:
        Dimension(fea::MessageBus& messageBus);
        void initialise(); 
        void addFocusPoint(const FocusPoint& focusPoint);
    private:
        fea::MessageBus& bus;
        Landscape landscape;
};
