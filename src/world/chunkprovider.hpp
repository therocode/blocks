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

class ChunkProvider :
    public fea::MessageReceiver<ChunkGenerationRequestedMessage,
                                BiomesLoadedMessage,
                                FrameMessage>
{
    public:
        ChunkProvider(fea::MessageBus& b);
        ~ChunkProvider();
        void handleMessage(const ChunkGenerationRequestedMessage& received) override;
        void handleMessage(const BiomesLoadedMessage& received) override;
        void handleMessage(const FrameMessage& received) override;
    private:
        ChunkDelivery generateChunk(WorldId worldId, const ChunkCoord& coordinate, const BiomeGrid& biomeData, const FieldMap& fieldData);
        fea::MessageBus& mBus;


        std::vector<std::future<ChunkDelivery>> mChunksToDeliver;
        
        std::unordered_map<BiomeId, Biome> mBiomes;

        ThreadPool mWorkerPool;
};
