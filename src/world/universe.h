#pragma once
#include "../blockstd.h"
#include "world.h"
#include "worldinterface.h"
#include "../entity/entitysystem.h"
#include <featherkit/messaging.h>
#include "../rendering/renderingmessages.h"
#include "worldmessages.h"
#include "chunkprovider.h"
#include "regionprovider.h"
#include "highlightmanager.h"

class Universe : 
        public fea::MessageReceiver<SetVoxelMessage>,
        public fea::MessageReceiver<RegionDeliverMessage>,
        public fea::MessageReceiver<ChunkDeliverMessage>,
        public fea::MessageReceiver<ChunkHighlightedMessage>,
        public fea::MessageReceiver<ChunkDehighlightedMessage>,
        public fea::MessageReceiver<RegionDeletedMessage>
{
    public:
        Universe(fea::MessageBus& messageBus);
		~Universe();
        void setup();
        void update();
        void destroy();
        virtual void handleMessage(const SetVoxelMessage& received);
        virtual void handleMessage(const RegionDeliverMessage& received);
        virtual void handleMessage(const ChunkDeliverMessage& received);
        virtual void handleMessage(const ChunkHighlightedMessage& received);
        virtual void handleMessage(const ChunkDehighlightedMessage& received);
        virtual void handleMessage(const RegionDeletedMessage& received);
        WorldInterface& getWorldInterface();
    private:
		glm::vec3 mCamPos, mCamDir;
        fea::MessageBus& mBus;
        World mStandardWorld;
        EntitySystem mEntitySystem;
        WorldInterface mWorldInterface;
        RegionProvider mRegionProvider;
        ChunkProvider mChunkProvider;
        HighlightManager mHighlightManager;
        ModManager mModManager;
};
