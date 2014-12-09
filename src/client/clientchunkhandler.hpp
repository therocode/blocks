#pragma once
#include "../world/chunkmap.hpp"
#include <fea/util.hpp>
#include "../networking/networkingprotocol.hpp"
#include "../rendering/renderingmessages.hpp"

class Chunk;

class ClientChunkHandler : fea::MessageReceiver<
    ClientChunksDeliveredMessage,
    VoxelUpdatedMessage,
    ClientChunkDeletedMessage>
{
    public:
        ClientChunkHandler(fea::MessageBus& bus);
        void addChunk(const ChunkCoord& coordinate, const Chunk& chunk);
        void updateVoxel(const VoxelCoord& voxelCoordinate, VoxelType type);
        void removeChunk(const ChunkCoord& coordinate);
        void handleMessage(const ClientChunksDeliveredMessage& received) override;
        void handleMessage(const VoxelUpdatedMessage& received) override;
        void handleMessage(const ClientChunkDeletedMessage& received) override;
    private:
        void updateChunk(const ChunkCoord& coordinate);
        fea::MessageBus& mBus;
        ChunkMap mLocalChunks;
};
