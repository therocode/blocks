#pragma once
#include "worldmessages.h"
#include "regionproviderinterface.h"
#include <featherkit/messaging.h>

class ChunkProvider : public fea::MessageReceiver<ChunkRequestedMessage>
{
    public:
        ChunkProvider(fea::MessageBus& bus, RegionProviderInterface& regionProvider);
        void handleMessage(const ChunkRequestedMessage& received) override;
    private:
        fea::MessageBus& mBus;
        RegionProviderInterface& mRegionProvider;
};
