#pragma once
#include <unordered_map>
#include "../utilities/glm.hpp"
#include <fea/util.hpp>
#include "../rendering/renderingmessages.hpp"
#include "worldmessages.hpp"
#include "worldparameters.hpp"
#include "worlddata.hpp"
#include "worldprovider.hpp"
#include "highlightmanager.hpp"
#include <memory>

struct WorldEntry
{
    WorldData worldData;

    HighlightManager highlightManager;
    ModManager modManager; //TBI
};

class WorldSystem : 
        public fea::MessageReceiver<SetVoxelMessage,
                                    ChunkDeliverMessage,
                                    HighlightEntityAddRequestedMessage,
                                    HighlightEntityMoveRequestedMessage,
                                    HighlightEntityRemoveRequestedMessage>
{
    public:
        WorldSystem(fea::MessageBus& messageBus);
        ~WorldSystem();
        void handleMessage(const SetVoxelMessage& received) override;
        void handleMessage(const ChunkDeliverMessage& received) override;
        void handleMessage(const HighlightEntityAddRequestedMessage& received) override;
        void handleMessage(const HighlightEntityMoveRequestedMessage& received) override;
        void handleMessage(const HighlightEntityRemoveRequestedMessage& received) override;
        const ChunkMap& getWorldVoxels(WorldId id) const;
    private:
        fea::MessageBus& mBus;
        WorldProvider mWorldProvider;

        //world data
        WorldId mNextId;
        std::unordered_map<std::string, WorldId> mIdentifierToIdMap;
        std::unordered_map<WorldId, WorldEntry> mWorlds;
};
