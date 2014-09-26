#pragma once
#include <unordered_map>
#include "../blockstd.hpp"
#include "worldinterface.hpp"
#include <fea/util.hpp>
#include "../rendering/renderingmessages.hpp"
#include "worldmessages.hpp"
#include "worldparameters.hpp"
#include "world.hpp"
#include <memory>

class WorldHolder : 
        public fea::MessageReceiver<SetVoxelMessage,
                                    RegionDeliverMessage,
                                    ChunkDeliverMessage>
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
        std::unordered_map<WorldId, std::unique_ptr<World>> mWorlds;
        std::unordered_map<std::string, WorldId> mWorldIds;
        WorldId mNextId;
        WorldInterface mWorldInterface;
};
