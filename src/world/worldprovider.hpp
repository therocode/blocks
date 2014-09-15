#pragma once
#include <fea/util.hpp>
#include "modmanager.hpp"
#include "generation/regiongenerator.hpp"
#include "generation/chunkgenerator.hpp"
#include "../application/applicationmessages.hpp"
#include <thread>
#include <mutex>

class Region;

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
        std::unordered_map<RegionCoord, Region> mRegions; //thread local copy of regions
        int32_t mThreadSleepInterval;
        int32_t mMaxChunkGenerationAmount;
        void generatorLoop();

        //thread input
        std::vector<ChunkCoord> mChunksToGenerate;
        std::mutex              mThreadInputMutex;
        bool                    mGenThreadActive;

        //thread storage
        std::mutex              mThreadMainMutex;
        std::vector<ChunkCoord> mChunkQueue;
        std::vector<std::pair<ChunkCoord, Chunk>> mFinishedChunks;
        std::vector<std::pair<RegionCoord, Region>> mFinishedRegions;

        //thread output
        std::vector<ChunkCoord> mChunksToDiscard;
        std::vector<RegionCoord> mRegionsToDiscard;
        std::vector<std::pair<RegionCoord, Region>> mRegionsToDeliver;
        std::vector<std::pair<ChunkCoord, Chunk>> mChunksToDeliver;
        std::mutex              mThreadOutputMutex;
};
