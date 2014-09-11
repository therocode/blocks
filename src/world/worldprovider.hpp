#pragma once
#include <fea/util.hpp>
#include "modmanager.hpp"
#include "generation/regiongenerator.hpp"
#include "generation/chunkgenerator.hpp"
#include "../application/applicationmessages.hpp"

class Region;

class WorldProvider :
    public ChunkRequestedMessageReceiver,
    public RegionDeletedMessageReceiver,
    public FrameMessageReceiver
{
    public:
        WorldProvider(fea::MessageBus& b, ModManager& modManager);
        ~WorldProvider();
        void handleMessage(const ChunkRequestedMessage& received) override;
        void handleMessage(const RegionDeletedMessage& received) override;
        void handleMessage(const FrameMessage& received) override;
    private:
        Chunk generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const;
        fea::MessageBus& mBus;
        ModManager& mModManager;
        RegionGenerator mRegionGenerator;
        ChunkGenerator mChunkGenerator;
        std::unordered_map<RegionCoord, Region> mRegions; //thread local copy of regions

        std::vector<std::pair<RegionCoord, Region>> mRegionsToDeliver;
        std::vector<std::pair<ChunkCoord, Chunk>> mChunksToDeliver;
};
