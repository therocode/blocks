#pragma once
#include <fea/util.hpp>
#include <unordered_map>
#include "../utilities/threadpool.hpp"
#include "worldmessages.hpp"
#include "../application/applicationmessages.hpp"
#include "worldbiomesettings.hpp"

using ChunkEntry = std::pair<WorldId, std::pair<ChunkCoord, Chunk>>;

struct BiomeDelivery
{
    WorldId id;
    BiomeRegionCoord coordinate;
    BiomeGrid biomeData;
};

class BiomeProvider :
    public fea::MessageReceiver<BiomesLoadedMessage,
                                WorldBiomeSettingsMessage,
                                BiomeRequestedMessage,
                                FrameMessage>
{
    public:
        BiomeProvider(fea::MessageBus& b);
        ~BiomeProvider();
        void handleMessage(const BiomesLoadedMessage& received) override;
        void handleMessage(const WorldBiomeSettingsMessage& received) override;
        void handleMessage(const BiomeRequestedMessage& received) override;
        void handleMessage(const FrameMessage& received) override;
    private:
        BiomeDelivery generateBiome(WorldId worldId, const BiomeRegionCoord& coordinate, const WorldBiomeSettings& settings);
        fea::MessageBus& mBus;

        ThreadPool mWorkerPool;
        std::unordered_map<BiomeIndex, Biome> mBiomes;
        std::unordered_map<WorldId, WorldBiomeSettings> mBiomeSettings;

        std::vector<std::future<BiomeDelivery>> mBiomesToDeliver;
};
