#pragma once
#include "../world/chunkmap.hpp"
#include "../world/highlightmanager.hpp"
#include "../networking/networkingprotocol.hpp"
#include "clientmessages.hpp"
#include <fea/util.hpp>

class ClientWorld : fea::MessageReceiver<
    ClientChunksDeliveredMessage,
    VoxelUpdatedMessage,
    ClientChunkDeletedMessage>
{
    public:
        ClientWorld(fea::MessageBus& bus);
        void updateVoxelLookAt();
        void handleMessage(const ClientChunksDeliveredMessage& received) override;
        void handleMessage(const VoxelUpdatedMessage& received) override;
        void handleMessage(const ClientChunkDeletedMessage& received) override;
    private:
        fea::MessageBus& mBus;
        HighlightManager mHighlightManager;
        std::unordered_map<WorldId, ChunkMap> mVoxels;
};
