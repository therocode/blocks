#pragma once
#include <fea/util.hpp>
#include "../utilities/threadpool.hpp"
#include "worldmessages.hpp"
#include "../application/applicationmessages.hpp"
#include "chunk.hpp"

using ChunkEntry = std::pair<WorldId, std::pair<ChunkCoord, Chunk>>;

struct ChunkDelivery
{
    WorldId id;
    ChunkCoord coordinate;
    Chunk chunk;
};

class WorldProvider :
    public fea::MessageReceiver<BiomesLoadedMessage,
                                ChunkRequestedMessage,
                                FrameMessage>
{
    public:
        WorldProvider(fea::MessageBus& b);
        ~WorldProvider();
        void handleMessage(const BiomesLoadedMessage& received) override;
        void handleMessage(const ChunkRequestedMessage& received) override;
        void handleMessage(const FrameMessage& received) override;
    private:
        ChunkDelivery generateChunk(WorldId worldId, const ChunkCoord& coordinate, const BiomeGrid& biomeData);
        fea::MessageBus& mBus;


        std::vector<std::future<ChunkDelivery>> mChunksToDeliver;
        
        std::unordered_map<BiomeIndex, Biome> mBiomes;

        ThreadPool mWorkerPool;
};
