#pragma once
#include <fea/util.hpp>
#include "regionstorageinterface.h"
#include "modmanager.h"
#include "generation/regiongenerator.h"
#include "generation/chunkgenerator.h"

class Region;

class WorldProvider :
    public ChunkRequestedMessageReceiver
{
    public:
        WorldProvider(fea::MessageBus& b, RegionStorageInterface& regionProvider, ModManager& modManager);
        ~WorldProvider();
        void handleMessage(const ChunkRequestedMessage& received) override;
        //Chunk generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const;
    private:
        Chunk generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const;
        fea::MessageBus& mBus;
        RegionStorageInterface& mRegionStorage;
        ModManager& mModManager;
        RegionGenerator mRegionGenerator;
        ChunkGenerator mChunkGenerator;
};
