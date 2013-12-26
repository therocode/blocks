#pragma once
#include "worldmessages.h"
#include "regionstorageinterface.h"
#include "generation/worldgenerator.h"
#include <featherkit/messaging.h>
#include "modmanager.h"

class ChunkProvider : public fea::MessageReceiver<ChunkRequestedMessage>
{
    public:
        ChunkProvider(fea::MessageBus& bus, RegionStorageInterface& regionProvider, ModManager& modManager);
        ~ChunkProvider();
        void handleMessage(const ChunkRequestedMessage& received) override;
    private:
        fea::MessageBus& mBus;
        RegionStorageInterface& mRegionStorage;
        ModManager& mModManager;
        WorldGenerator mWorldGenerator;
};
