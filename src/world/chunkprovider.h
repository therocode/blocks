#pragma once
#include "worldmessages.h"
#include "regionstorageinterface.h"
#include "generation/worldgenerator.h"
#include <featherkit/messaging.h>

class ChunkProvider : public fea::MessageReceiver<ChunkRequestedMessage>
{
    public:
        ChunkProvider(fea::MessageBus& bus, RegionStorageInterface& regionProvider);
        ~ChunkProvider();
        void handleMessage(const ChunkRequestedMessage& received) override;
    private:
        fea::MessageBus& mBus;
        RegionStorageInterface& mRegionStorage;
        WorldGenerator mWorldGenerator;
};
