#pragma once
#include "entitycontroller.h"
#include "../../application/applicationmessages.h"
#include "../../input/inputmessages.h"
#include "../../world/chunk.h"

class PlayerController : 
    public EntityController,
    public fea::MessageReceiver<PlayerJoinedMessage>,
    public fea::MessageReceiver<PlayerActionMessage>,
    public fea::MessageReceiver<PlayerPitchYawMessage>
{
    public:
        PlayerController(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~PlayerController();
        virtual void inspectEntity(fea::WeakEntityPtr entity);
        virtual void removeEntity(fea::EntityId id);
        virtual void onFrame() override;
        void handleMessage(const PlayerJoinedMessage& received);
        void handleMessage(const PlayerActionMessage& received);
        void handleMessage(const PlayerPitchYawMessage& received);
    private:
        void playerEntersChunk(size_t playerId, const ChunkCoordinate& chunk);
        void updateVoxelLookAt(size_t playerId);
        std::unordered_map<size_t, fea::WeakEntityPtr> mPlayerEntities;
        std::map<size_t, float> mPlayerThrottles;
        std::map<size_t, ChunkCoordinate> mPlayerChunks;
        std::map<size_t, glm::vec3> mPlayerFacings;
        bool mForward;
};
