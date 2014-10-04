#pragma once
#include "entitycontroller.hpp"
#include "../../application/applicationmessages.hpp"
#include "../../input/inputmessages.hpp"
#include "../entitymessages.hpp"

using ChunkCoord = glm::i64vec3;

class PlayerController : 
    public EntityController,
    public fea::MessageReceiver<PlayerJoinedGameMessage,
                                PlayerLeftGameMessage,
                                PlayerActionMessage,
                                PlayerMoveDirectionMessage,
                                PlayerMoveActionMessage,
                                PlayerPitchYawMessage,
                                EntityMovedMessage>
{
    public:
        PlayerController(fea::MessageBus& bus, GameInterface& worldInterface);
        virtual void inspectEntity(fea::WeakEntityPtr entity);
        virtual void removeEntity(fea::EntityId id);
        virtual void onFrame(int dt) override;
        void handleMessage(const PlayerJoinedGameMessage& received);
        void handleMessage(const PlayerLeftGameMessage& received);
        void handleMessage(const PlayerActionMessage& received);
        void handleMessage(const PlayerMoveDirectionMessage& received);
        void handleMessage(const PlayerMoveActionMessage& received);
        void handleMessage(const PlayerPitchYawMessage& received);
        void handleMessage(const EntityMovedMessage& received);
    private:
        void playerEntersChunk(size_t playerId, const ChunkCoord& chunk);
        void updateVoxelLookAt(size_t playerId);
        std::unordered_map<size_t, fea::WeakEntityPtr> mPlayerEntities;
};
