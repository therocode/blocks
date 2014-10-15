#pragma once
#include <unordered_map>
#include "../utilities/glm.hpp"
#include <fea/util.hpp>
#include "../rendering/renderingmessages.hpp"
#include "worldmessages.hpp"
#include "worldprovider.hpp"
#include "biomeprovider.hpp"
#include "worldentry.hpp"

class WorldSystem : 
        public fea::MessageReceiver<SetVoxelMessage,
                                    BiomeGeneratedMessage,
                                    ChunkGeneratedMessage,
                                    HighlightEntityAddRequestedMessage,
                                    HighlightEntityMoveRequestedMessage,
                                    HighlightEntityRemoveRequestedMessage,
                                    ChunksRequestedMessage>
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
        const ChunkMap& getWorldVoxels(WorldId id) const;
        WorldId worldIdentifierToId(const std::string& identifier) const;
    private:
        void createWorld(const WorldParameters& parameters);
        fea::MessageBus& mBus;
        WorldProvider mWorldProvider;
        BiomeProvider mBiomeProvider;

        //biome data
        BiomeIndex mNextBiomeIndex;
        std::unordered_map<BiomeIndex, Biome> mBiomes;
        std::unordered_map<std::string, BiomeIndex> mBiomeIdentifierToIdMap;

        //worlds
        WorldId mNextId;
        std::unordered_map<std::string, WorldId> mIdentifierToIdMap;
        std::unordered_map<WorldId, WorldEntry> mWorlds;
};
