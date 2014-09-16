#include "worldprovider.hpp"
#include "region.hpp"
#include "../application/applicationmessages.hpp"

const std::string logName = "world_gen";

WorldProvider::WorldProvider(fea::MessageBus& b)
    :
    mBus(b),
    mGeneratorThread(&WorldProvider::generatorLoop, this),
    mThreadSleepInterval(5),
    mMaxChunkGenerationAmount(5),
    mGenThreadActive(true)
{
    mBus.send(LogMessage{"Started world generation thread", logName, LogLevel::INFO});
    mBus.addSubscriber<ChunkRequestedMessage>(*this);
    mBus.addSubscriber<RegionDeletedMessage>(*this);
    mBus.addSubscriber<FrameMessage>(*this);
    mBus.addSubscriber<HaltChunkAndRegionGenerationMessage>(*this);
}

WorldProvider::~WorldProvider()
{
    mGenThreadActive = false;
    mBus.send(LogMessage{"Shutting down world generation thread...", logName, LogLevel::INFO});
    mGeneratorThread.join();
    mBus.send(LogMessage{"World generation thread shut down", logName, LogLevel::INFO});

    mBus.removeSubscriber<ChunkRequestedMessage>(*this);
    mBus.removeSubscriber<RegionDeletedMessage>(*this);
    mBus.removeSubscriber<FrameMessage>(*this);
    mBus.removeSubscriber<HaltChunkAndRegionGenerationMessage>(*this);
}

void WorldProvider::handleMessage(const ChunkRequestedMessage& received)
{
    //add chunk to load to other thread
    std::lock_guard<std::mutex> lock(mThreadInputMutex);
    mChunksToGenerate.push_back(std::pair<int32_t, std::pair<WorldId, ChunkCoord>>(received.prio, {received.worldId, received.coordinate}));       
}

void WorldProvider::handleMessage(const RegionDeletedMessage& received)
{
    std::lock_guard<std::mutex> lock(mThreadMainMutex);
    mRegions.erase({received.worldId, received.coordinate}); 
}

void WorldProvider::handleMessage(const FrameMessage& received)
{
    std::vector<RegionEntry> regions;
    std::vector<ChunkEntry> chunks;

    //prevent thread collision
    {
        std::lock_guard<std::mutex> lock(mThreadOutputMutex);
        regions = mRegionsToDeliver;
        chunks = mChunksToDeliver;

        mRegionsToDeliver.clear();
        mChunksToDeliver.clear();
    }

    if(regions.size() > 0)
    {
        for(const auto& region : regions)
        {
            mBus.send(RegionDeliverMessage{region.first, region.second.first, std::move(region.second.second)});
        }
    }

    if(chunks.size() > 0)
    {
        for(auto& chunk : chunks)
        {
            mBus.send(ChunkDeliverMessage{chunk.first, chunk.second.first, std::move(chunk.second.second)}); //sends the finished chunk to be kept by whatever system
        }
    }
}

void WorldProvider::handleMessage(const HaltChunkAndRegionGenerationMessage& received)
{
    {
        std::lock_guard<std::mutex> inLock(mThreadInputMutex);
        std::lock_guard<std::mutex> outLock(mThreadOutputMutex);

        for(size_t i = 0; i < mChunksToGenerate.size(); i++)
        {
            if(mChunksToGenerate[i].second == std::pair<WorldId, ChunkCoord>(received.worldId, received.chunkCoordinate))
                mChunksToGenerate.erase(mChunksToGenerate.begin() + i);
        }

        for(size_t i = 0; i < mChunksToDeliver.size(); i++)
        {
            if(mChunksToDeliver[i].second.first == received.chunkCoordinate)
            {
                mChunksToDeliver.erase(mChunksToDeliver.begin() + i);
                break;
            }
        }

        mChunksToDiscard.push_back({received.worldId, received.chunkCoordinate});

        if(received.regionCoordinate)
        {

            for(size_t i = 0; i < mRegionsToDeliver.size(); i++)
            {
                if(mRegionsToDeliver[i].second.first == *received.regionCoordinate)
                {
                    mRegionsToDeliver.erase(mRegionsToDeliver.begin() + i);
                    break;
                }
            }

            mRegionsToDiscard.push_back({received.worldId, *received.regionCoordinate});
        }
    }
}

void WorldProvider::generatorLoop()
{
    while(mGenThreadActive)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(mThreadSleepInterval));

        //check for new chunks to generate
        {
            std::lock_guard<std::mutex> lock(mThreadInputMutex);
            mChunkQueue.insert(mChunkQueue.end(), mChunksToGenerate.begin(), mChunksToGenerate.end());
            mChunksToGenerate.clear();
        }


        //perform generation
        {
            std::lock_guard<std::mutex> lock(mThreadMainMutex);

            std::sort(mChunkQueue.begin(), mChunkQueue.end(), [] 
                    (const std::pair<int32_t, std::pair<WorldId, ChunkCoord>>& a, const std::pair<int32_t, std::pair<WorldId, ChunkCoord>>& b)
                    {
                        return a.first < b.first;
                    });

            int32_t amountGenerated = 0;
            for(size_t i = 0; i < mChunkQueue.size(); i++)
            {
                WorldId worldId = mChunkQueue[i].second.first;
                const auto& chunkCoordinate = mChunkQueue[i].second.second;
                RegionCoord regionCoordinate = chunkToRegion(chunkCoordinate);

                if(mRegions.count({worldId, regionCoordinate}) == 0)
                {
                    Region newRegion = mRegionGenerator.generateRegion(regionCoordinate);
                    mRegions.emplace(std::pair<WorldId, RegionCoord>(worldId, regionCoordinate), newRegion);
                    mFinishedRegions.push_back({worldId, {regionCoordinate, newRegion}});
                }

                const Region& region = mRegions.at({worldId, regionCoordinate});

                Chunk newChunk = mChunkGenerator.generateChunk(chunkCoordinate, region);

                mFinishedChunks.push_back({worldId, {chunkCoordinate, newChunk}});
                amountGenerated++;

                if(amountGenerated == mMaxChunkGenerationAmount)
                    break;
            }

            mChunkQueue.erase(mChunkQueue.begin(), mChunkQueue.begin() + amountGenerated);
        }

        //deliver finished chunks
        {
            std::lock_guard<std::mutex> lock1(mThreadOutputMutex);
            std::lock_guard<std::mutex> lock2(mThreadMainMutex);   //this is needed for mRegions to be able to remove regions from it

            mChunksToDeliver.insert(mChunksToDeliver.end(), mFinishedChunks.begin(), mFinishedChunks.end());
            mRegionsToDeliver.insert(mRegionsToDeliver.end(), mFinishedRegions.begin(), mFinishedRegions.end());
            mFinishedChunks.clear();
            mFinishedRegions.clear();

            for(const auto& chunk : mChunksToDiscard)
            {
                for(size_t i = 0; i < mChunkQueue.size(); i++)
                {
                    if(mChunkQueue[i].second == chunk)
                    {
                        mChunkQueue.erase(mChunkQueue.begin() + i);
                        break;
                    }
                }
            }
            for(const auto& chunk : mChunksToDiscard)
            {
                for(size_t i = 0; i < mChunksToDeliver.size(); i++)
                {
                    if(mChunksToDeliver[i].first == chunk.first && mChunksToDeliver[i].second.first == chunk.second)
                    {
                        mChunksToDeliver.erase(mChunksToDeliver.begin() + i);
                        break;
                    }
                }
            }
            for(const auto& region : mRegionsToDiscard)
            {
                for(size_t i = 0; i < mRegionsToDeliver.size(); i++)
                {
                    if(mRegionsToDeliver[i].first == region.first && mRegionsToDeliver[i].second.first == region.second)
                    {
                        mRegionsToDeliver.erase(mRegionsToDeliver.begin() + i);
                        mRegions.erase(region);
                        break;
                    }
                }
            }
            mChunksToDiscard.clear();
            mRegionsToDiscard.clear();
        }
    }
}
