#include <algorithm>
#include "biomeprovider.hpp"
#include "fieldgenerator.hpp"
#include "../utilities/interpolators.hpp"

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
    auto bound = std::bind(&BiomeProvider::generateBiomeData, this, received.worldId, received.coordinate);
    mBiomesToDeliver.push_back(mWorkerPool.enqueue(bound, 0));
}

void BiomeProvider::handleMessage(const FrameMessage& received)
{
    for(auto iter = mBiomesToDeliver.begin(); iter != mBiomesToDeliver.end();)
    {
        if(iter->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            BiomeDelivery delivery = iter->get();
            mBus.send(BiomeGeneratedMessage({delivery.id, delivery.coordinate, std::move(delivery.biomeData), std::move(delivery.fields)}));
            iter = mBiomesToDeliver.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

BiomeDelivery BiomeProvider::generateBiomeData(WorldId worldId, BiomeRegionCoord coordinate)
{
    const WorldBiomeSettings& settings = mBiomeSettings.at(worldId);
    FieldGenerator fieldGenerator;

    FieldMap fieldGrids;

    for(const auto& field : settings.fields)
    {
        FieldGrid newGrid(biomeRegionWidth + biomeInterpolationWidth * 2, biomeDownSamplingAmount);

        fieldGenerator.fill(coordinate, newGrid, field.noiseType);

        newGrid.setInterpolator(Interpolator<float>::nearestNeigbor);

        fieldGrids.emplace(field.id, newGrid);
    }

    BiomeGrid biomeData = generateBiomes(worldId, coordinate, fieldGrids);

    BiomeDelivery delivery{worldId, coordinate, std::move(biomeData), std::move(fieldGrids)};
    return delivery;
}

BiomeGrid BiomeProvider::generateBiomes(WorldId worldId, const BiomeRegionCoord& coordinate, const FieldMap& fields) const
{
    BiomeGrid result(biomeRegionWidth + biomeInterpolationWidth * 2, biomeDownSamplingAmount);
    uint32_t size = result.getInnerSize();

    const auto& biomeSettings = mBiomeSettings.at(worldId);

    //collect all possible biomes
    std::vector<const Biome*> worldBiomes;
    for(const auto& biomeIndex : biomeSettings.biomes)
    {
        worldBiomes.push_back(&mBiomes.at(biomeIndex));
    }

    std::vector<const Biome*> approvedPointBiomes;

    uint32_t selectorId = -1;

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

                    for(const auto& field : biomeSettings.fields)
                    {
                        if(field.isSelector)
                        {
                            selectorId = field.id;
                            continue;
                        }
                        else if(biome.mRequirements.count(field.id) == 0)
                        {
                            continue;
                        }

                        if(!biome.mRequirements.at(field.id).isWithin(fields.at(field.id).getInner({x, y, z})))
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

                FEA_ASSERT(selectorId != -1, "selector noise missing!");
                float selectPercent = fields.at(selectorId).getInner({x, y, z});
                BiomeId selectedBiome = approvedPointBiomes[(uint32_t)((float)approvedPointBiomes.size() * selectPercent)]->mId;

                result.setInner({x, y, z}, selectedBiome); 
            }
        }
    }

    result.setInterpolator(Interpolator<BiomeId>::nearestNeigbor);
    return result;
}
