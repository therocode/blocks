#pragma once
#include "entitycontroller.h"
#include "../../application/applicationmessages.h"
#include "../../input/inputmessages.h"
#include "../entitymessages.h"
#include "../../world/chunk.h"

class PlayerController : 
    public EntityController,
    public fea::MessageReceiver<PlayerJoinedMessage>,
    public fea::MessageReceiver<PlayerDisconnectedMessage>,
    public fea::MessageReceiver<PlayerActionMessage>,
    public fea::MessageReceiver<PlayerMoveDirectionMessage>,
    public fea::MessageReceiver<PlayerMoveActionMessage>,
    public fea::MessageReceiver<PlayerPitchYawMessage>,
    public fea::MessageReceiver<EntityMovedMessage>
{
    public:
        PlayerController(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~PlayerController();
        virtual void inspectEntity(fea::WeakEntityPtr entity);
        virtual void removeEntity(fea::EntityId id);
        virtual void onFrame(int dt) override;
        void handleMessage(const PlayerJoinedMessage& received);
        void handleMessage(const PlayerDisconnectedMessage& received);
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
