#include "worldprovider.hpp"
#include "regiondatafragment.hpp"

const std::string logName = "world_gen";
const int32_t threadAmount = 3;

WorldProvider::WorldProvider(fea::MessageBus& b)
    :
    mBus(b),
    mWorkerPool(threadAmount)
{
    //these biome definitions should eventually be located in external files
    BiomeStorage biomes;
    biomes.addBiome(Biome("snowpeak", 24, 1.0f, 1.0f, 1.0f,        Range(0.0f, 0.1f), Range(0.0f, 1.0f),  Range(0.85f, 1.0f)));
    biomes.addBiome(Biome("peak", 6, 0.6f, 0.6f, 0.6f,            Range(0.1f, 1.0f), Range(0.0f, 1.0f),  Range(0.85f, 1.0f)));

    biomes.addBiome(Biome("baremountain", 6, 0.5f, 0.5f, 0.5f,    Range(0.0f, 1.0f), Range(0.0f, 1.0f),  Range(0.68f, 0.93f)));
    biomes.addBiome(Biome("grassmountain", 25, 0.4f, 0.6f, 0.4f,   Range(0.3f, 1.0f), Range(0.3f, 1.0f),  Range(0.68f, 0.93f)));
    biomes.addBiome(Biome("snowymountain", 24, 1.0f, 1.0f, 1.0f,   Range(0.0f, 0.3f), Range(0.4f, 1.0f),  Range(0.68f, 0.93f)));

    biomes.addBiome(Biome("forest", 26, 0.0f, 0.4f, 0.0f,         Range(0.35f, 1.0f),Range(0.60f, 1.0f),  Range(0.2f, 0.73f)));
    biomes.addBiome(Biome("taiga", 27, 0.0f, 0.6f, 0.3f,          Range(0.1f, 0.5f), Range(0.60f, 1.0f),  Range(0.2f, 0.73f)));
    biomes.addBiome(Biome("plains", 1, 0.2f, 0.7f, 0.0f,          Range(0.25f, 1.0f),Range(0.50f, 0.7f),  Range(0.2f, 0.73f)));
    biomes.addBiome(Biome("steppe", 28, 0.3f, 0.6f, 0.0f,          Range(0.25f, 1.0f), Range(0.20f, 0.60f), Range(0.2f, 0.73f)));
    biomes.addBiome(Biome("snowfield", 29, 1.3f, 1.3f, 1.3f,       Range(0.1f, 0.3f), Range(0.20f, 1.0f),  Range(0.2f, 0.73f)));
    biomes.addBiome(Biome("arcticfield", 30, 1.42f, 1.42f, 1.42f,  Range(0.0f, 0.13f), Range(0.20f, 1.0f),  Range(0.2f, 0.73f)));
    biomes.addBiome(Biome("xeric", 31, 1.0f, 0.8f, 0.2f,           Range(0.35f, 1.0f), Range(0.05f, 0.25f), Range(0.2f, 0.73f)));
    biomes.addBiome(Biome("desert", 32, 1.3f, 1.3f, 0.2f,          Range(0.35f, 1.0f), Range(0.0f, 0.15f),  Range(0.2f, 0.73f)));
    biomes.addBiome(Biome("coldxeric", 33, 1.f, 0.7f, 0.4f,       Range(0.0f, 0.4f), Range(0.05f, 0.25f),  Range(0.2f, 0.73f)));
    biomes.addBiome(Biome("colddesert", 34, 1.2f, 0.9f, 0.5f,      Range(0.0f, 0.4f), Range(0.0f, 0.15f),  Range(0.2f, 0.73f)));

    biomes.addBiome(Biome("ocean", 35, 0.0f, 0.0f, 1.0f,           Range(0.2f, 1.0f), Range(0.0f, 1.0f),  Range(0.0f, 0.2f)));
    biomes.addBiome(Biome("arctic ocean", 36, 0.0f, 0.9f, 1.0f,    Range(0.0f, 0.2f), Range(0.0f, 1.0f),  Range(0.0f, 0.2f)));

    mBus.send(LogMessage{"Started world generation thread pool with " + std::to_string(threadAmount) + " threads", logName, LogLevel::INFO});

    fea::subscribe(mBus, *this);

    //set up thread storages
    for(const auto& id : mWorkerPool.getThreadIds())
        mThreadStorage.emplace(id, biomes);
}

WorldProvider::~WorldProvider()
{
    mBus.send(LogMessage{"Shutting down world generation thread pool", logName, LogLevel::INFO});
}

void WorldProvider::handleMessage(const ChunkRequestedMessage& received)
{
    //add chunk to load to other thread
    //std::cout << "requesting chunk " << glm::to_string((glm::ivec3)received.coordinate) << " to world " << received.worldId << "\n";
    auto bound = std::bind(&WorldProvider::generateChunk, this, received.worldId, received.coordinate, received.regionData);
    mChunksToDeliver.push_back(mWorkerPool.enqueue(bound, received.prio));
}

void WorldProvider::handleMessage(const FrameMessage& received)
{
    for(auto iter = mChunksToDeliver.begin(); iter != mChunksToDeliver.end();)
    {
        if(iter->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            ChunkDelivery delivery = iter->get();
            mBus.send(ChunkDeliverMessage({delivery.id, delivery.coordinate, std::move(delivery.chunk)}));
            iter = mChunksToDeliver.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

ChunkDelivery WorldProvider::generateChunk(WorldId worldId, const ChunkCoord& coordinate, const RegionDataFragment& regionFragment)
{
    ChunkDelivery delivery;
    ChunkGenerator generator;
    
    delivery.id = worldId;
    delivery.coordinate = coordinate;

    delivery.chunk = generator.generateChunk(coordinate, regionFragment, mThreadStorage.at(std::this_thread::get_id()));

    return delivery;
}
