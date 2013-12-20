#pragma once
#include <featherkit/messaging.h>
#include "worldmessages.h"
#include "generation/regiongenerator.h"

class RegionProvider : public fea::MessageReceiver<RegionNeededMessage>
{
    public:
        RegionProvider(fea::MessageBus& bus);
        ~RegionProvider();
        void handleMessage(const RegionNeededMessage& received) override;
    private:
        fea::MessageBus& mBus;
        RegionGenerator mRegionGenerator;
};
