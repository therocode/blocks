#pragma once
#include "../blockstd.hpp"
#include "world.hpp"
#include "worldinterface.hpp"
#include "../entity/entitysystem.hpp"
#include <fea/util.hpp>
#include "../rendering/renderingmessages.hpp"
#include "worldmessages.hpp"
#include "worldprovider.hpp"
#include "highlightmanager.hpp"
#include "regionmanager.hpp"

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
        WorldProvider mWorldProvider;
        HighlightManager mHighlightManager;
        ModManager mModManager;
        RegionManager mRegionManager;
};
