#include <algorithm>
#include "biomeprovider.hpp"
#include "regiondatafragment.hpp"
#include "generation/biomegenerator.hpp"
#include "fieldgenerator.hpp"

const std::string logName = "world_gen";
const int32_t threadAmount = 3;

BiomeProvider::BiomeProvider(fea::MessageBus& b)
    :
    mBus(b),
    mWorkerPool(threadAmount)
{
    //these biome definitions should eventually be located in external files
    //BiomeStorage biomes;
    //biomes.addBiome(Biome("snowpeak", 24, 1.0f, 1.0f, 1.0f,        Range(0.0f, 0.1f), Range(0.0f, 1.0f),  Range(0.85f, 1.0f)));
    //biomes.addBiome(Biome("peak", 6, 0.6f, 0.6f, 0.6f,            Range(0.1f, 1.0f), Range(0.0f, 1.0f),  Range(0.85f, 1.0f)));

    //biomes.addBiome(Biome("baremountain", 6, 0.5f, 0.5f, 0.5f,    Range(0.0f, 1.0f), Range(0.0f, 1.0f),  Range(0.68f, 0.93f)));
    //biomes.addBiome(Biome("grassmountain", 25, 0.4f, 0.6f, 0.4f,   Range(0.3f, 1.0f), Range(0.3f, 1.0f),  Range(0.68f, 0.93f)));
    //biomes.addBiome(Biome("snowymountain", 24, 1.0f, 1.0f, 1.0f,   Range(0.0f, 0.3f), Range(0.4f, 1.0f),  Range(0.68f, 0.93f)));

    //biomes.addBiome(Biome("forest", 26, 0.0f, 0.4f, 0.0f,         Range(0.35f, 1.0f),Range(0.60f, 1.0f),  Range(0.2f, 0.73f)));
    //biomes.addBiome(Biome("taiga", 27, 0.0f, 0.6f, 0.3f,          Range(0.1f, 0.5f), Range(0.60f, 1.0f),  Range(0.2f, 0.73f)));
    //biomes.addBiome(Biome("plains", 1, 0.2f, 0.7f, 0.0f,          Range(0.25f, 1.0f),Range(0.50f, 0.7f),  Range(0.2f, 0.73f)));
    //biomes.addBiome(Biome("steppe", 28, 0.3f, 0.6f, 0.0f,          Range(0.25f, 1.0f), Range(0.20f, 0.60f), Range(0.2f, 0.73f)));
    //biomes.addBiome(Biome("snowfield", 29, 1.3f, 1.3f, 1.3f,       Range(0.1f, 0.3f), Range(0.20f, 1.0f),  Range(0.2f, 0.73f)));
    //biomes.addBiome(Biome("arcticfield", 30, 1.42f, 1.42f, 1.42f,  Range(0.0f, 0.13f), Range(0.20f, 1.0f),  Range(0.2f, 0.73f)));
    //biomes.addBiome(Biome("xeric", 31, 1.0f, 0.8f, 0.2f,           Range(0.35f, 1.0f), Range(0.05f, 0.25f), Range(0.2f, 0.73f)));
    //biomes.addBiome(Biome("desert", 32, 1.3f, 1.3f, 0.2f,          Range(0.35f, 1.0f), Range(0.0f, 0.15f),  Range(0.2f, 0.73f)));
    //biomes.addBiome(Biome("coldxeric", 33, 1.f, 0.7f, 0.4f,       Range(0.0f, 0.4f), Range(0.05f, 0.25f),  Range(0.2f, 0.73f)));
    //biomes.addBiome(Biome("colddesert", 34, 1.2f, 0.9f, 0.5f,      Range(0.0f, 0.4f), Range(0.0f, 0.15f),  Range(0.2f, 0.73f)));

    //biomes.addBiome(Biome("ocean", 35, 0.0f, 0.0f, 1.0f,           Range(0.2f, 1.0f), Range(0.0f, 1.0f),  Range(0.0f, 0.2f)));
    //biomes.addBiome(Biome("arctic ocean", 36, 0.0f, 0.9f, 1.0f,    Range(0.0f, 0.2f), Range(0.0f, 1.0f),  Range(0.0f, 0.2f)));

    mBus.send(LogMessage{"Started biome generation thread pool with " + std::to_string(threadAmount) + " threads", logName, LogLevel::INFO});

    fea::subscribe(mBus, *this);

    //set up thread storages
    //for(const auto& id : mWorkerPool.getThreadIds())
    //    mThreadStorage.emplace(id, biomes);
}

BiomeProvider::~BiomeProvider()
{
    mBus.send(LogMessage{"Shutting down biome generation thread pool", logName, LogLevel::INFO});
}

void BiomeProvider::handleMessage(const BiomesLoadedMessage& received)
{
    mBus.send(LogMessage{std::to_string(received.biomes.size()) + " biomes registered in biome generator", logName, LogLevel::INFO});
    mBiomes = received.biomes;

    for(const auto& biome : mBiomes)
    {
        mBiomeNameToIndex.emplace(biome.second.mName, biome.first);
    }
}

void BiomeProvider::handleMessage(const WorldBiomeSettingsMessage& received)
{
    mBus.send(LogMessage{std::to_string(received.settings.biomes.size()) + " biomes registered in world " + std::to_string(received.worldId), logName, LogLevel::INFO});
    mBiomeSettings.emplace(received.worldId, received.settings);
}

void BiomeProvider::handleMessage(const BiomeRequestedMessage& received)
{
    //add biome to load to other thread
    FEA_ASSERT(mBiomeSettings.count(received.worldId) != 0, "Got a biome to generate but there are no settings registered for this world ID " + std::to_string(received.worldId));
    auto bound = std::bind(&BiomeProvider::generateBiome, this, received.worldId, received.coordinate);
    mBiomesToDeliver.push_back(mWorkerPool.enqueue(bound, 0));
}

void BiomeProvider::handleMessage(const FrameMessage& received)
{
    for(auto iter = mBiomesToDeliver.begin(); iter != mBiomesToDeliver.end();)
    {
        if(iter->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            BiomeDelivery delivery = iter->get();
            mBus.send(BiomeDeliveredMessage({delivery.id, delivery.coordinate, std::move(delivery.biomeData)}));
            iter = mBiomesToDeliver.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

BiomeDelivery BiomeProvider::generateBiome(WorldId worldId, BiomeRegionCoord coordinate)
{
    const WorldBiomeSettings& settings = mBiomeSettings.at(worldId);
    FieldGenerator fieldGenerator;
    BiomeGenerator generator;
    BiomeGrid biomeData(biomeRegionWidth, 4);  //4 is amount of downsampling

    std::unordered_map<std::string, FieldGrid> fieldGrids;

    for(const auto& field : settings.fields)
    {
        FieldGrid newGrid(biomeRegionWidth, 4);  //4 is amount of downsampling

        if(field.name != "selector")
        {
            fieldGenerator.fill(coordinate, newGrid, NoiseType::SIMPLEX);
        }
        else
        {
            fieldGenerator.fill(coordinate, newGrid, NoiseType::VORONOI);
        }

        fieldGrids.emplace(field.name, newGrid);
    }

    std::vector<Biome*> worldBiomes;

    //collect all possible biomes
    for(const auto& biomeIndex : mBiomeSettings.at(worldId).biomes)
    {
        worldBiomes.push_back(&mBiomes.at(biomeIndex));
    }

    uint32_t size = biomeData.getInnerSize();

    std::vector<const Biome*> approvedPointBiomes;
    for(uint32_t z = 0; z < size; z++)
    {
        for(uint32_t y = 0; y < size; y++)
        {
            for(uint32_t x = 0; x < size; x++)
            {
                approvedPointBiomes.clear();

                for(uint32_t i = 0; i < worldBiomes.size(); i++)
                {
                    const Biome& biome = *worldBiomes[i];

                    bool include = true;

                    for(const auto& field : fieldGrids)
                    {
                        if(field.first == "selector")
                        {
                            continue;
                        }
                        else if(biome.mRequirements.count(field.first) == 0)
                        {
                            continue;
                        }

                        if(!biome.mRequirements.at(field.first).isWithin(field.second.getInner({x, y, z})))
                        {
                            include = false;
                            break;
                        }
                    }

                    if(include)
                    {
                        approvedPointBiomes.push_back(&biome);
                    }
                };

                float selectPercent = fieldGrids.at("selector").getInner({x, y, z});
                BiomeIndex selectedBiome = mBiomeNameToIndex.at(approvedPointBiomes[(uint32_t)((float)approvedPointBiomes.size() * selectPercent)]->mName);

                biomeData.setInner({x, y, z}, selectedBiome); 
            }
        }
    }
    ////filter out biomes that match 
    //std::remove_if(worldBiomes.begin(), worldBiomes.end(), [] (const Biome* biome) 
    //        {
    //            for(const auto& requirement : biome->mRequirements)

    //            return false;
    //        });

    BiomeDelivery delivery{worldId, coordinate, std::move(biomeData)};
    return delivery;
}
