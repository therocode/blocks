#include "chunkprovider.hpp"
#include "generation/chunkgenerator.hpp"

const std::string logName = "world_gen";
const int32_t threadAmount = 3;

ChunkProvider::ChunkProvider(fea::MessageBus& b)
    :
    mBus(b),
    mWorkerPool(threadAmount)
{
    mBus.send(LogMessage{"Started world generation thread pool with " + std::to_string(threadAmount) + " threads", logName, LogLevel::INFO});

    fea::subscribe(mBus, *this);
}

ChunkProvider::~ChunkProvider()
{
    mBus.send(LogMessage{"Shutting down world generation thread pool", logName, LogLevel::INFO});
}

void ChunkProvider::handleMessage(const BiomesLoadedMessage& received)
{
    mBiomes = received.biomes;
}

void ChunkProvider::handleMessage(const ChunkGenerationRequestedMessage& received)
{
    //add chunk to load to other thread
    auto bound = std::bind(&ChunkProvider::generateChunk, this, received.worldId, received.coordinate, received.biomeData, received.fieldData);
    mChunksToDeliver.push_back(mWorkerPool.enqueue(bound, received.prio));
}

void ChunkProvider::handleMessage(const FrameMessage& received)
{
    for(auto iter = mChunksToDeliver.begin(); iter != mChunksToDeliver.end();)
    {
        if(iter->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            ChunkDelivery delivery = iter->get();
            mBus.send(ChunkGeneratedMessage({delivery.id, delivery.coordinate, std::move(delivery.chunk)}));
            iter = mChunksToDeliver.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

ChunkDelivery ChunkProvider::generateChunk(WorldId worldId, const ChunkCoord& coordinate, const BiomeGrid& biomeData, const FieldMap& fieldData)
{
    ChunkDelivery delivery;
    ChunkGenerator generator;
    
    delivery.id = worldId;
    delivery.coordinate = coordinate;

    delivery.chunk = generator.generateChunk(coordinate, biomeData, fieldData, mBiomes);

    return delivery;
}
