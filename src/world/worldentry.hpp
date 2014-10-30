#pragma once
#include <unordered_map>
#include <vector>
#include <fea/util.hpp>
#include "worlddata.hpp"
#include "worldparameters.hpp"
#include "../utilities/gridnotifier.hpp"
#include "highlightmanager.hpp"
#include "modmanager.hpp"
#include "explorationmanager.hpp"
#include "../application/applicationmessages.hpp"

class WorldEntry :
    public fea::MessageReceiver<FrameMessage>
{
    public:
        WorldEntry(fea::MessageBus& bus, const std::unordered_map<BiomeId, Biome>& biomes, WorldId id, const std::string& identifier, const WorldData& data, const std::string& path);
        WorldEntry(WorldEntry&& other);
        ~WorldEntry();
        void addHighlightEntity(uint32_t id, const ChunkCoord& location, uint32_t radius);
        void moveHighlightEntity(uint32_t id, const ChunkCoord& location);
        void removeHighlightEntity(uint32_t id);
        void deliverBiome(const BiomeRegionCoord& coordinate, const BiomeGrid& biomeData, const FieldMap& fields);
        void deliverChunk(const ChunkCoord& coordinate, const Chunk& chunk);
        const ChunkMap& getChunkMap() const;
        ChunkMap& getChunkMap();
        void setVoxelType(const VoxelCoord& voxelCoordinate, VoxelType type);
        void chunksRequested(const std::vector<ChunkCoord>& coordinates);
        const std::string& getIdentifier() const;
        void handleMessage(const FrameMessage& received) override;
    private:
        void activateChunk(const ChunkCoord& chunk);
        void deactivateChunk(const ChunkCoord& chunk);
        void requestChunk(const ChunkCoord& chunk);
        void applyDifferenceAsMods(const ChunkCoord& coordinate, const VoxelTypeArray& before, const VoxelTypeArray& after);
        fea::MessageBus& mBus;
        const std::unordered_map<BiomeId, Biome>& mBiomes;
        WorldId mId;
        std::string mIdentifier;
        WorldData mWorldData;
        GridNotifier<biomeRegionWidthInChunks> mBiomeGridNotifier;
        HighlightManager mHighlightManager;
        ModManager mModManager;
		ExplorationManager mExplorationManager;
		uint64_t mCurrentFrameNumber;
		
        std::unordered_map<BiomeRegionCoord, std::vector<ChunkCoord>> mPendingChunksToRequests;
};
