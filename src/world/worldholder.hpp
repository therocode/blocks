#pragma once
#include <unordered_map>
#include "../blockstd.hpp"
#include "world.hpp"
#include "worldinterface.hpp"
#include "../entity/entitysystem.hpp"
#include <fea/util.hpp>
#include "../rendering/renderingmessages.hpp"
#include "worldmessages.hpp"
#include "worldprovider.hpp"
#include "regionmanager.hpp"

class WorldHolder : 
        public fea::MessageReceiver<SetVoxelMessage>,
        public fea::MessageReceiver<RegionDeliverMessage>,
        public fea::MessageReceiver<ChunkDeliverMessage>,
        public fea::MessageReceiver<ChunkHighlightedMessage>,
        public fea::MessageReceiver<ChunkDehighlightedMessage>
{
    public:
        WorldHolder(fea::MessageBus& messageBus);
		~WorldHolder();
        void setup();
        void update();
        void destroy();
        virtual void handleMessage(const SetVoxelMessage& received);
        virtual void handleMessage(const RegionDeliverMessage& received);
        virtual void handleMessage(const ChunkDeliverMessage& received);
        virtual void handleMessage(const ChunkHighlightedMessage& received);
        virtual void handleMessage(const ChunkDehighlightedMessage& received);
        WorldInterface& getWorldInterface();
    private:
        fea::MessageBus& mBus;
        std::unordered_map<std::string, World> mWorlds;
        EntitySystem mEntitySystem;
        WorldInterface mWorldInterface;
        WorldProvider mWorldProvider;
        RegionManager mRegionManager;
};
