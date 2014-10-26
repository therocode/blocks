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
    FieldMap fields;
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
        BiomeDelivery generateBiomeData(WorldId worldId, BiomeRegionCoord coordinate);
        fea::MessageBus& mBus;

        std::unordered_map<BiomeId, Biome> mBiomes;
        std::unordered_map<WorldId, WorldBiomeSettings> mBiomeSettings;

        std::vector<std::future<BiomeDelivery>> mBiomesToDeliver;

        ThreadPool mWorkerPool; //threads last so that their stuff don't get destructed
};
