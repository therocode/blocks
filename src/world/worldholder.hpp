#pragma once
#include <unordered_map>
#include "../blockstd.hpp"
#include "world.hpp"
#include "worldinterface.hpp"
#include <fea/util.hpp>
#include "../rendering/renderingmessages.hpp"
#include "worldmessages.hpp"
#include "worldparameters.hpp"

class WorldHolder : 
        public fea::MessageReceiver<SetVoxelMessage>,
        public fea::MessageReceiver<RegionDeliverMessage>,
        public fea::MessageReceiver<ChunkDeliverMessage>
{
    public:
        WorldHolder(fea::MessageBus& messageBus, EntitySystem& entitySystem);
		~WorldHolder();
        virtual void handleMessage(const SetVoxelMessage& received);
        virtual void handleMessage(const RegionDeliverMessage& received);
        virtual void handleMessage(const ChunkDeliverMessage& received);
        WorldInterface& getWorldInterface();
        void addWorld(const WorldParameters& worldParameters);
    private:
        fea::MessageBus& mBus;
        std::unordered_map<WorldId, World> mWorlds;
        std::unordered_map<std::string, WorldId> mWorldIds;
        WorldId mNextId;
        WorldInterface mWorldInterface;
};
