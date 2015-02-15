#pragma once
#include <unordered_map>
#include "../utilities/glm.hpp"
#include "../utilities/idmapper.hpp"
#include <fea/util.hpp>
#include "../rendering/renderingmessages.hpp"
#include "worldmessages.hpp"
#include "chunkprovider.hpp"
#include "biomeprovider.hpp"
#include "worldentry.hpp"
#include "../resources/resourcemessages.hpp"

class WorldSystem : 
        public fea::MessageReceiver<SetVoxelMessage,
                                    BiomeGeneratedMessage,
                                    ChunkGeneratedMessage,
                                    HighlightEntityAddRequestedMessage,
                                    HighlightEntityMoveRequestedMessage,
                                    HighlightEntityRemoveRequestedMessage,
                                    ChunksRequestedMessage,
									ResourceDeliverMessage<std::vector<WorldParameters>>>
{
    public:
        WorldSystem(fea::MessageBus& messageBus);
        ~WorldSystem();
        void handleMessage(const SetVoxelMessage& received) override;
        void handleMessage(const BiomeGeneratedMessage& received) override;
        void handleMessage(const ChunkGeneratedMessage& received) override;
        void handleMessage(const HighlightEntityAddRequestedMessage& received) override;
        void handleMessage(const HighlightEntityMoveRequestedMessage& received) override;
        void handleMessage(const HighlightEntityRemoveRequestedMessage& received) override;
        void handleMessage(const ChunksRequestedMessage& received) override;
        void handleMessage(const ResourceDeliverMessage<std::vector<WorldParameters>>& received) override;
        const ChunkMap& getWorldVoxels(WorldId id) const;
        bool hasWorld(const std::string& identifier) const;
        WorldId worldIdentifierToId(const std::string& identifier) const;
        const std::string& worldIdToIdentifier(WorldId id) const;
    private:
        void createWorld(const WorldParameters& parameters);
        fea::MessageBus& mBus;
        ChunkProvider mChunkProvider;
        BiomeProvider mBiomeProvider;

        //biome data
        IdMapper<std::string> mFieldIds;
        IdMapper<std::string> mBiomeIds;
        std::unordered_map<BiomeId, Biome> mBiomes;

        //worlds
		std::string mWorldPath;
        WorldId mNextId;
        std::unordered_map<std::string, WorldId> mIdentifierToIdMap;
        std::unordered_map<WorldId, WorldEntry> mWorlds;
};
