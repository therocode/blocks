#pragma once
#include <fea/util.hpp>
#include "modmanager.hpp"
#include "generation/chunkgenerator.hpp"
#include "../utilities/threadpool.hpp"
#include "generation/biomestorage.hpp"
#include "worldmessages.hpp"
#include "../application/applicationmessages.hpp"

using ChunkEntry = std::pair<WorldId, std::pair<ChunkCoord, Chunk>>;

struct ChunkDelivery
{
    WorldId id;
    ChunkCoord coordinate;
    Chunk chunk;
};

class WorldProvider :
    public fea::MessageReceiver<ChunkRequestedMessage,
                                FrameMessage>
{
    public:
        WorldProvider(fea::MessageBus& b);
        ~WorldProvider();
        void handleMessage(const ChunkRequestedMessage& received) override;
        void handleMessage(const FrameMessage& received) override;
    private:
        ChunkDelivery generateChunk(WorldId worldId, const ChunkCoord& coordinate, const RegionDataFragment& regionFragment);
        fea::MessageBus& mBus;
        ChunkGenerator mChunkGenerator;

        ThreadPool mWorkerPool;
        std::unordered_map<std::thread::id, BiomeStorage> mThreadStorage;

        std::vector<std::future<ChunkDelivery>> mChunksToDeliver;
};
