#include "worldprovider.hpp"
#include "region.hpp"
#include "../application/applicationmessages.hpp"

const std::string logName = "world_gen";

WorldProvider::WorldProvider(fea::MessageBus& b, ModManager& modManager)
    :
    mBus(b),
    mModManager(modManager),
    mGeneratorThread(&WorldProvider::generatorLoop, this),
    mThreadSleepInterval(5),
    mMaxChunkGenerationAmount(5),
    mGenThreadActive(true)
{
    mBus.send(LogMessage{"Started world generation thread", logName, LogLevel::INFO});
    mBus.addSubscriber<ChunkRequestedMessage>(*this);
    mBus.addSubscriber<RegionDeletedMessage>(*this);
    mBus.addSubscriber<FrameMessage>(*this);
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
}

void WorldProvider::handleMessage(const ChunkRequestedMessage& received)
{
    std::cout << "requested new chunk " << glm::to_string((glm::ivec3)received.coordinate) << "\n";
    //add chunk to load to other thread
    std::lock_guard<std::mutex> lock(mThreadInputMutex);
    mChunksToGenerate.push_back(received.coordinate);       
}

void WorldProvider::handleMessage(const RegionDeletedMessage& received)
{
    mRegions.erase(received.coordinate); //this is not thread safe, must hax
}

void WorldProvider::handleMessage(const FrameMessage& received)
{
    std::vector<std::pair<RegionCoord, Region>> regions;
    std::vector<std::pair<ChunkCoord, Chunk>> chunks;

    //prevent thread collision
    {
        std::lock_guard<std::mutex> lock(mThreadOutputMutex);
        regions = mRegionsToDeliver;
        chunks = mChunksToDeliver;

        //std::cout << "fetched done stuff! had " << mRegionsToDeliver.size() << " regions to deliver and " << mChunksToDeliver.size() << " chunks\n";

        mRegionsToDeliver.clear();
        mChunksToDeliver.clear();
    }

    if(regions.size() > 0)
    {
        for(const auto& region : regions)
        {
            mBus.send(RegionDeliverMessage{region.first, std::move(region.second)});
        }
    }

    if(chunks.size() > 0)
    {
        for(const auto& chunk : chunks)
        {
            mBus.send(ChunkDeliverMessage{chunk.first, std::move(chunk.second)}); //sends the finished chunk to be kept by whatever system

            uint64_t timestamp = 0; //get proper timestamp later
            mBus.send(ChunkLoadedMessage{chunk.first, timestamp}); //the now fully initialised chunk is announced to the rest of the game. should it be here?
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
            //std::cout << "fetching new jobs, I had " << mChunksToGenerate.size() << " to fetch\n";
            mChunksToGenerate.clear();
        }


        //perform generation
        int32_t amountGenerated = 0;
        for(size_t i = 0; i < mChunkQueue.size(); i++)
        {
            const auto& chunkCoordinate = mChunkQueue[i];
            //std::cout << "making a chunk\n";
            RegionCoord regionCoordinate = chunkToRegion(chunkCoordinate);

            if(mRegions.count(regionCoordinate) == 0)
            {
                Region newRegion = mRegionGenerator.generateRegion(regionCoordinate);
                mRegions.emplace(regionCoordinate, newRegion);
                mFinishedRegions.push_back({regionCoordinate, newRegion});
            }

            const Region& region = mRegions.at(regionCoordinate);

            Chunk newChunk = mChunkGenerator.generateChunk(chunkCoordinate, region);

            mModManager.loadMods(newChunk);

            mFinishedChunks.push_back({chunkCoordinate, newChunk});
            amountGenerated++;
            
            if(amountGenerated == mMaxChunkGenerationAmount)
                break;
        }

        mChunkQueue.erase(mChunkQueue.begin(), mChunkQueue.begin() + amountGenerated);

        //deliver finished chunks
        {
            std::lock_guard<std::mutex> lock(mThreadOutputMutex);
            mChunksToDeliver.insert(mChunksToDeliver.end(), mFinishedChunks.begin(), mFinishedChunks.end());
            mRegionsToDeliver.insert(mRegionsToDeliver.end(), mFinishedRegions.begin(), mFinishedRegions.end());
            mFinishedChunks.clear();
            mFinishedRegions.clear();
            //std::cout << "delivering. will deliver " << mChunksToDeliver.size() << " chinks and " << mRegionsToDeliver.size() << " regions\n";
        }
    }
}
