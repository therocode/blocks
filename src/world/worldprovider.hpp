#pragma once
#include <fea/util.hpp>
#include "modmanager.hpp"
#include "generation/regiongenerator.hpp"
#include "generation/chunkgenerator.hpp"
#include "../application/applicationmessages.hpp"
#include "../utilities/threadpool.hpp"
#include "generation/biomestorage.hpp"
#include "region.hpp"

namespace std 
{
    template<>
    struct hash<std::pair<WorldId, RegionCoord>>
    {   
        public:
            std::size_t operator()(std::pair<WorldId, RegionCoord> const& worldRegion) const 
            {   
                return (worldRegion.second.x | (worldRegion.second.y << 1) | worldRegion.first << 1); 
            }
    };
}


using ChunkEntry = std::pair<WorldId, std::pair<ChunkCoord, Chunk>>;
using RegionEntry = std::pair<WorldId, std::pair<RegionCoord, Region>>;

struct RegionDelivery
{
    WorldId id;
    RegionCoord coordinate;
    Region region;
};

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
        void handleMessage(const RegionRequestedMessage& received) override;
        void handleMessage(const ChunkRequestedMessage& received) override;
        void handleMessage(const FrameMessage& received) override;
    private:
        RegionDelivery generateRegion(WorldId worldId, const RegionCoord& coordinate);
        ChunkDelivery generateChunk(WorldId worldId, const ChunkCoord& coordinate, const RegionDataFragment& regionFragment);
        fea::MessageBus& mBus;
        RegionGenerator mRegionGenerator;
        ChunkGenerator mChunkGenerator;

        ThreadPool mWorkerPool;
        std::unordered_map<std::thread::id, BiomeStorage> mThreadStorage;

        std::vector<std::future<RegionDelivery>> mRegionsToDeliver;
        std::vector<std::future<ChunkDelivery>> mChunksToDeliver;
};
