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
    mBiomes.addBiome(Biome("snowpeak", 24, 1.0f, 1.0f, 1.0f,        Range(0.0f, 0.1f), Range(0.0f, 1.0f),  Range(0.85f, 1.0f)));
    mBiomes.addBiome(Biome("peak", 6, 0.6f, 0.6f, 0.6f,            Range(0.1f, 1.0f), Range(0.0f, 1.0f),  Range(0.85f, 1.0f)));

    mBiomes.addBiome(Biome("baremountain", 6, 0.5f, 0.5f, 0.5f,    Range(0.0f, 1.0f), Range(0.0f, 1.0f),  Range(0.68f, 0.93f)));
    mBiomes.addBiome(Biome("grassmountain", 25, 0.4f, 0.6f, 0.4f,   Range(0.3f, 1.0f), Range(0.3f, 1.0f),  Range(0.68f, 0.93f)));
    mBiomes.addBiome(Biome("snowymountain", 24, 1.0f, 1.0f, 1.0f,   Range(0.0f, 0.3f), Range(0.4f, 1.0f),  Range(0.68f, 0.93f)));

    mBiomes.addBiome(Biome("forest", 26, 0.0f, 0.4f, 0.0f,         Range(0.35f, 1.0f),Range(0.60f, 1.0f),  Range(0.2f, 0.73f)));
    mBiomes.addBiome(Biome("taiga", 27, 0.0f, 0.6f, 0.3f,          Range(0.1f, 0.5f), Range(0.60f, 1.0f),  Range(0.2f, 0.73f)));
    mBiomes.addBiome(Biome("plains", 1, 0.2f, 0.7f, 0.0f,          Range(0.25f, 1.0f),Range(0.50f, 0.7f),  Range(0.2f, 0.73f)));
    mBiomes.addBiome(Biome("steppe", 28, 0.3f, 0.6f, 0.0f,          Range(0.25f, 1.0f), Range(0.20f, 0.60f), Range(0.2f, 0.73f)));
    mBiomes.addBiome(Biome("snowfield", 29, 1.3f, 1.3f, 1.3f,       Range(0.1f, 0.3f), Range(0.20f, 1.0f),  Range(0.2f, 0.73f)));
    mBiomes.addBiome(Biome("arcticfield", 30, 1.42f, 1.42f, 1.42f,  Range(0.0f, 0.13f), Range(0.20f, 1.0f),  Range(0.2f, 0.73f)));
    mBiomes.addBiome(Biome("xeric", 31, 1.0f, 0.8f, 0.2f,           Range(0.35f, 1.0f), Range(0.05f, 0.25f), Range(0.2f, 0.73f)));
    mBiomes.addBiome(Biome("desert", 32, 1.3f, 1.3f, 0.2f,          Range(0.35f, 1.0f), Range(0.0f, 0.15f),  Range(0.2f, 0.73f)));
    mBiomes.addBiome(Biome("coldxeric", 33, 1.f, 0.7f, 0.4f,       Range(0.0f, 0.4f), Range(0.05f, 0.25f),  Range(0.2f, 0.73f)));
    mBiomes.addBiome(Biome("colddesert", 34, 1.2f, 0.9f, 0.5f,      Range(0.0f, 0.4f), Range(0.0f, 0.15f),  Range(0.2f, 0.73f)));

    mBiomes.addBiome(Biome("ocean", 35, 0.0f, 0.0f, 1.0f,           Range(0.2f, 1.0f), Range(0.0f, 1.0f),  Range(0.0f, 0.2f)));
    mBiomes.addBiome(Biome("arctic ocean", 36, 0.0f, 0.9f, 1.0f,    Range(0.0f, 0.2f), Range(0.0f, 1.0f),  Range(0.0f, 0.2f)));

    mBus.send(LogMessage{"Started world generation thread", logName, LogLevel::INFO});
    mBus.addSubscriber<RegionRequestedMessage>(*this);
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

    mBus.removeSubscriber<RegionRequestedMessage>(*this);
    mBus.removeSubscriber<ChunkRequestedMessage>(*this);
    mBus.removeSubscriber<RegionDeletedMessage>(*this);
    mBus.removeSubscriber<FrameMessage>(*this);
    mBus.removeSubscriber<HaltChunkAndRegionGenerationMessage>(*this);
}

void WorldProvider::handleMessage(const RegionRequestedMessage& received)
{
    //add region to load to other thread
    std::cout << "requesting region " << glm::to_string((glm::ivec2)received.coordinate) << " to world " << received.worldId << "\n";
    std::lock_guard<std::mutex> lock(mThreadInputMutex);
    mRegionsToGenerate.push_back({received.worldId, received.coordinate});       
}

void WorldProvider::handleMessage(const ChunkRequestedMessage& received)
{
    //add chunk to load to other thread
    //std::cout << "requesting chunk " << glm::to_string((glm::ivec3)received.coordinate) << " to world " << received.worldId << "\n";
    std::lock_guard<std::mutex> lock(mThreadInputMutex);
    mChunksToGenerate.push_back(std::pair<int32_t, std::pair<WorldId, ChunkCoord>>(received.prio, {received.worldId, received.coordinate}));       
}

void WorldProvider::handleMessage(const RegionDeletedMessage& received)
{
    std::cout << "region " << glm::to_string((glm::ivec2)received.coordinate) << " queued for deletion from generator regarding world " << received.worldId << "\n";
    std::lock_guard<std::mutex> lock(mThreadInputMutex);
    mRegionsToDelete.push_back({received.worldId, received.coordinate}); 
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
            std::cout << "Region delivered " << glm::to_string((glm::ivec2)region.second.first) << " to world " << region.first << "\n";
            mBus.send(RegionDeliverMessage{region.first, region.second.first, std::move(region.second.second)});
        }
    }

    if(chunks.size() > 0)
    {
        for(auto& chunk : chunks)
        {
            //std::cout << "Chunk delivered" << glm::to_string((glm::ivec3)chunk.second.first) << " to world " << chunk.first << "\n";
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
            std::cout << "Discarding chunk " << glm::to_string((glm::ivec3) received.chunkCoordinate) << " to world " << received.worldId << "\n";
            if(mChunksToGenerate[i].second == std::pair<WorldId, ChunkCoord>(received.worldId, received.chunkCoordinate))
                mChunksToGenerate.erase(mChunksToGenerate.begin() + i);
        }

        for(size_t i = 0; i < mChunksToDeliver.size(); i++)
        {
            std::cout << "Discarding chunk " << glm::to_string((glm::ivec3) received.chunkCoordinate) << " to world " << received.worldId << "\n";
            if(mChunksToDeliver[i].second.first == received.chunkCoordinate && mChunksToDeliver[i].first == received.worldId)
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
                if(mRegionsToDeliver[i].second.first == *received.regionCoordinate && mRegionsToDeliver[i].first == received.worldId)
                {
                    std::cout << "Disccccccarded region " << glm::to_string((glm::ivec2)*received.regionCoordinate) << " to world " << mRegionsToDeliver[i].first << "\n";
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

        //check for new chunks and regions to generate/delete
        {
            std::lock_guard<std::mutex> lock(mThreadInputMutex);
            mRegionQueue.insert(mRegionQueue.end(), mRegionsToGenerate.begin(), mRegionsToGenerate.end());
            mRegionsToGenerate.clear();
            mChunkQueue.insert(mChunkQueue.end(), mChunksToGenerate.begin(), mChunksToGenerate.end());
            mChunksToGenerate.clear();

            for(const auto& region : mRegionsToDelete)
            {
                std::cout << "erasing region from regionstodelete. region: " << glm::to_string((glm::ivec2) region.second) << " in world " << region.first << "\n";
                mRegions.erase(region);
            }

            mRegionsToDelete.clear();
        }


        //perform generation
        {
            //region generation is prioritised and should be delivered ASAP, hence chunk generation is skipped if a region is generated

            bool regionGenerated = false;
            if(mRegionQueue.size() > 0)
            {
                auto region = mRegionQueue.back();
                mRegionQueue.pop_back();

                FEA_ASSERT(mRegions.count(region) == 0, "Generation thread is generating region " + glm::to_string(glm::ivec2(region.second)) + " in world " + std::to_string(region.first));
                mRegionGenerator.setSeed(region.first + 50);
                Region newRegion = mRegionGenerator.generateRegion(region.second);
                mRegions.emplace(region, newRegion);
                mFinishedRegions.push_back({region.first, {region.second, newRegion}});
                std::cout << "made region " << glm::to_string((glm::ivec2)region.second) << " to world " << region.first << "\n";

                regionGenerated = true;
            }

            //chunk generation is skipped if a region was generated
            if(!regionGenerated)
            {
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

                    FEA_ASSERT(mRegions.count({worldId, regionCoordinate}) != 0, "Trying to generate chunk " + glm::to_string((glm::ivec3)chunkCoordinate) + " to world " + std::to_string(worldId) + " belonging to region " + glm::to_string((glm::ivec2)regionCoordinate) + " but that region does not exist\n");
                    
                    const Region& region = mRegions.at({worldId, regionCoordinate});

                    Chunk newChunk = mChunkGenerator.generateChunk(chunkCoordinate, region, mBiomes);

                    mFinishedChunks.push_back({worldId, {chunkCoordinate, newChunk}});
                    amountGenerated++;

                    if(amountGenerated == mMaxChunkGenerationAmount)
                        break;
                }

                mChunkQueue.erase(mChunkQueue.begin(), mChunkQueue.begin() + amountGenerated);
            }
        }

        //deliver finished chunks and regions. Discarded chunks and regions must not be delivered
        {
            std::lock_guard<std::mutex> lock1(mThreadOutputMutex);

            mRegionsToDeliver.insert(mRegionsToDeliver.end(), mFinishedRegions.begin(), mFinishedRegions.end());
            mFinishedRegions.clear();
            mChunksToDeliver.insert(mChunksToDeliver.end(), mFinishedChunks.begin(), mFinishedChunks.end());
            mFinishedChunks.clear();

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
                std::cout << "looking to discard region " << glm::to_string((glm::ivec2)region.second) << "\n";
                for(size_t i = 0; i < mRegionQueue.size(); i++)
                {
                    if(mRegionQueue[i] == region)
                    {
                        mRegionQueue.erase(mRegionQueue.begin() + i);
                        break;
                    }
                }
                for(size_t i = 0; i < mRegionsToDeliver.size(); i++)
                {
                    if(mRegionsToDeliver[i].first == region.first && mRegionsToDeliver[i].second.first == region.second)
                    {
                        std::cout << "Discarrrrrrded region from delivery " << glm::to_string((glm::ivec2)region.second) << " to world " << region.first << "\n";
                        mRegionsToDeliver.erase(mRegionsToDeliver.begin() + i);
                        //print chunk queue and their regions
                        break;
                    }
                    else
                    {
                        std::cout << "didn't discard it\n";
                    }
                }

                if(mRegions.count(region) != 0)
                {
                    std::cout << "erased it internally\n";
                    mRegions.erase(region);
                    //print chunk queue and their regions
                }
            }

            mChunksToDiscard.clear();
            mRegionsToDiscard.clear();
        }
    }
}
