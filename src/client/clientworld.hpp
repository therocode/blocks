#pragma once
#include "../world/chunkmap.hpp"
#include "../world/highlightmanager.hpp"
#include "../world/worldmessages.hpp"
#include "clientmessages.hpp"
#include <fea/util.hpp>

class ClientWorld : fea::MessageReceiver<
    ChunksDataDeliveredMessage,
    VoxelSetMessage,
    ChunkDeletedMessage,
    HighlightEntityAddRequestedMessage>
{
    public:
        ClientWorld(fea::MessageBus& bus);
        void handleMessage(const ChunksDataDeliveredMessage& received) override;
        void handleMessage(const VoxelSetMessage& received) override;
        void handleMessage(const ChunkDeletedMessage& received) override;
        void handleMessage(const HighlightEntityAddRequestedMessage& received) override;
        const ChunkMap& getVoxels(WorldId worldId) const;
    private:
        void updateChunk(WorldId id, const ChunkCoord& coordinate);
        fea::MessageBus& mBus;
        HighlightManager mHighlightManager;
        std::unordered_map<WorldId, ChunkMap> mVoxels;
};
