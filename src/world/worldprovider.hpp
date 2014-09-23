#pragma once
#include <fea/util.hpp>
#include "modmanager.hpp"
#include "generation/regiongenerator.hpp"
#include "generation/chunkgenerator.hpp"
#include "../application/applicationmessages.hpp"
#include "../utilities/threadpool.hpp"

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


class Region;

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
    public RegionRequestedMessageReceiver,
    public ChunkRequestedMessageReceiver,
    public FrameMessageReceiver
{
    public:
        WorldProvider(fea::MessageBus& b);
        ~WorldProvider();
        void handleMessage(const RegionRequestedMessage& received) override;
        void handleMessage(const ChunkRequestedMessage& received) override;
        void handleMessage(const FrameMessage& received) override;
    private:
        fea::MessageBus& mBus;
        RegionGenerator mRegionGenerator;
        ChunkGenerator mChunkGenerator;

        BiomeStorage mBiomes;
        ThreadPool mWorkerPool;

        std::vector<std::future<RegionDelivery>> mRegionsToDeliver;
        std::vector<std::future<ChunkDelivery>> mChunksToDeliver;
};
