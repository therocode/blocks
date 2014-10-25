#pragma once
#include "entitycontroller.hpp"
#include "../../application/applicationmessages.hpp"
#include "../../input/inputmessages.hpp"
#include "../entitymessages.hpp"
#include "../playermessages.hpp"

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
        PlayerController(fea::MessageBus& bus, GameInterface& gameInterface);
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
        std::unordered_map<fea::EntityId, size_t> mEntityIdToPlayerId;
        GameInterface& mGameInterface;
};
