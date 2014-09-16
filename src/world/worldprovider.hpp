#pragma once
#include <fea/util.hpp>
#include "modmanager.hpp"
#include "generation/regiongenerator.hpp"
#include "generation/chunkgenerator.hpp"
#include "../application/applicationmessages.hpp"
#include <thread>
#include <mutex>

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

class WorldProvider :
    public ChunkRequestedMessageReceiver,
    public RegionDeletedMessageReceiver,
    public FrameMessageReceiver,
    public HaltChunkAndRegionGenerationMessageReceiver
{
    public:
        WorldProvider(fea::MessageBus& b);
        ~WorldProvider();
        void handleMessage(const ChunkRequestedMessage& received) override;
        void handleMessage(const RegionDeletedMessage& received) override;
        void handleMessage(const FrameMessage& received) override;
        void handleMessage(const HaltChunkAndRegionGenerationMessage& received) override;
    private:
        Chunk generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const;
        fea::MessageBus& mBus;
        RegionGenerator mRegionGenerator;
        ChunkGenerator mChunkGenerator;

        //thread
        std::thread mGeneratorThread;
        std::unordered_map<std::pair<WorldId, RegionCoord>, Region> mRegions; //thread local copy of regions
        int32_t mThreadSleepInterval;
        int32_t mMaxChunkGenerationAmount;
        void generatorLoop();

        //thread input
        std::vector<std::pair<WorldId, ChunkCoord>> mChunksToGenerate;
        std::mutex              mThreadInputMutex;
        bool                    mGenThreadActive;

        //thread storage
        std::mutex              mThreadMainMutex;
        std::vector<std::pair<WorldId, ChunkCoord>> mChunkQueue;
        std::vector<ChunkEntry> mFinishedChunks;
        std::vector<RegionEntry> mFinishedRegions;

        //thread output
        std::vector<std::pair<WorldId, ChunkCoord>> mChunksToDiscard;
        std::vector<std::pair<WorldId, RegionCoord>> mRegionsToDiscard;
        std::vector<RegionEntry> mRegionsToDeliver;
        std::vector<ChunkEntry> mChunksToDeliver;
        std::mutex              mThreadOutputMutex;
};
