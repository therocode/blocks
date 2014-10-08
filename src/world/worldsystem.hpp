#pragma once
#include <unordered_map>
#include "../utilities/glm.hpp"
#include <fea/util.hpp>
#include "../rendering/renderingmessages.hpp"
#include "worldmessages.hpp"
#include "worldparameters.hpp"
#include "voxelstorage.hpp"
#include "worldprovider.hpp"
#include <memory>

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
        const VoxelStorage& getWorldVoxels(WorldId id) const;
    private:
        fea::MessageBus& mBus;
        std::unordered_map<WorldId, VoxelStorage> mWorldVoxels;
        std::unordered_map<std::string, WorldId> mIdentifierToIdMap;
        WorldId mNextId;
        WorldProvider mWorldProvider;
};
