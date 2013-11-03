#include "playercontroller.h"
#include "../../world/worldinterface.h"
#include "../../rendering/renderingmessages.h"
#include "moveaction.h"
#include <iostream>

PlayerController::PlayerController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
    mBus.addMessageSubscriber<PlayerJoinedMessage>(*this);
    mBus.addMessageSubscriber<PlayerActionMessage>(*this);
    mBus.addMessageSubscriber<PlayerMoveDirectionMessage>(*this);
    mBus.addMessageSubscriber<PlayerMoveActionMessage>(*this);
    mBus.addMessageSubscriber<PlayerPitchYawMessage>(*this);
    mBus.addMessageSubscriber<EntityMovedMessage>(*this);
}

PlayerController::~PlayerController()
{
    mBus.removeMessageSubscriber<PlayerJoinedMessage>(*this);
    mBus.removeMessageSubscriber<PlayerActionMessage>(*this);
    mBus.removeMessageSubscriber<PlayerMoveDirectionMessage>(*this);
    mBus.removeMessageSubscriber<PlayerMoveActionMessage>(*this);
    mBus.removeMessageSubscriber<PlayerPitchYawMessage>(*this);
    mBus.removeMessageSubscriber<EntityMovedMessage>(*this);
}

void PlayerController::inspectEntity(fea::WeakEntityPtr entity)
{
}

void PlayerController::removeEntity(fea::EntityId id)
{
}

void PlayerController::onFrame()
{
}

void PlayerController::handleMessage(const PlayerJoinedMessage& received)
{
    size_t playerId;
    glm::vec3 position;

    std::tie(playerId, position) = received.data;

    fea::WeakEntityPtr playerEntity = mWorldInterface.spawnEntity("Player", position);
    mPlayerEntities.emplace(playerId, playerEntity);
    playerEntity.lock()->setAttribute<ChunkCoordinate>("current_chunk", worldToChunk(position));

    mBus.sendMessage<PlayerConnectedToEntityMessage>(PlayerConnectedToEntityMessage(playerId, playerEntity.lock()->getId()));
}

void PlayerController::handleMessage(const PlayerActionMessage& received)
{
    size_t playerId;
    InputAction action;

    std::tie(playerId, action) = received.data;

    if(action == FORWARDS)
    {
            //fea::EntityPtr player = mPlayerEntities.at(playerId).lock();
            //player->setAttribute<MoveAction>("move_action", MoveAction::STANDING);
    }
    else if(action == JUMP)
    {
        mBus.sendMessage<EntityJumpMessage>(EntityJumpMessage(mPlayerEntities.at(playerId).lock()->getId(), true));
    }else if(action == STOPJUMP)
	{
		mBus.sendMessage<EntityJumpMessage>(EntityJumpMessage(mPlayerEntities.at(playerId).lock()->getId(), false));
	}
}

void PlayerController::handleMessage(const PlayerMoveDirectionMessage& received)
{
    size_t playerId;
    MoveDirection direction;

    std::tie(playerId, direction) = received.data;

    mPlayerEntities.at(playerId).lock()->setAttribute<MoveDirection>("move_direction", direction);
}

void PlayerController::handleMessage(const PlayerMoveActionMessage& received)
{
    size_t playerId;
    MoveAction moveAction;

    std::tie(playerId, moveAction) = received.data;

    mPlayerEntities.at(playerId).lock()->setAttribute<MoveAction>("move_action", moveAction);
}

void PlayerController::handleMessage(const PlayerPitchYawMessage& received) //movement controller ni the future
{
	size_t playerId;

    float pitch;
    float yaw;

    std::tie(playerId, pitch, yaw) = received.data;

    auto playerEntry = mPlayerEntities.find(playerId);
    if(playerEntry != mPlayerEntities.end())
    {
        fea::EntityPtr entity = playerEntry->second.lock();
		float newPitch = entity->getAttribute<float>("pitch");
		newPitch += glm::radians(pitch);
		
		if(newPitch >= glm::pi<float>() * 0.5f)
			newPitch = glm::pi<float>() * 0.5f - 0.001f;
		if(newPitch <= -glm::pi<float>() * 0.5f)
			newPitch = -glm::pi<float>() * 0.5f + 0.001f;
		
		
			
        entity->setAttribute<float>("pitch", newPitch);
        entity->addToAttribute<float>("yaw", glm::radians(yaw));

        float newYaw = entity->getAttribute<float>("yaw");
		//printf("Pitch: %f, and yaw: %f\n", newPitch, newYaw);
        mBus.sendMessage<RotateGfxEntityMessage>(RotateGfxEntityMessage(playerEntry->second.lock()->getId(), newPitch, newYaw));
        updateVoxelLookAt(playerId);
    }
}

void PlayerController::handleMessage(const EntityMovedMessage& received)
{
    size_t id;

    std::tie(id, std::ignore, std::ignore) = received.data;

    if(mPlayerEntities.find(id) != mPlayerEntities.end())
    {
        updateVoxelLookAt(id);

        fea::EntityPtr entity = mPlayerEntities.at(id).lock();
        //updating current chunk
        glm::vec3 position = entity->getAttribute<glm::vec3>("position");

        if(worldToChunk(position) != entity->getAttribute<ChunkCoordinate>("current_chunk"))
        {
            playerEntersChunk(id, worldToChunk(position));
        }
    }

}

void PlayerController::playerEntersChunk(size_t playerId, const ChunkCoordinate& chunk)
{
    mBus.sendMessage<PlayerEntersChunkMessage>(PlayerEntersChunkMessage(playerId, chunk));
    mPlayerEntities.at(playerId).lock()->setAttribute<ChunkCoordinate>("current_chunk", chunk);
}

void PlayerController::updateVoxelLookAt(size_t playerId)
{
    fea::EntityPtr entity = mPlayerEntities.at(playerId).lock();


    float pitch = entity->getAttribute<float>("pitch");
    float yaw = entity->getAttribute<float>("yaw");
    glm::vec3 position = entity->getAttribute<glm::vec3>("position");
  
	glm::vec3 direction = glm::vec3(glm::cos(pitch)*glm::sin(yaw), glm::sin(pitch), glm::cos(pitch) * glm::cos(yaw));
	if(glm::length2(direction) != 0)
		direction = glm::normalize(direction) * 10.0f;

	glm::vec3 block = mWorldInterface.getVoxelAtRay(position + glm::vec3(0, 0.6f, 0), direction);

    if(block != entity->getAttribute<glm::vec3>("block_facing"))
    {
        entity->setAttribute<glm::vec3>("block_facing", block);
        mBus.sendMessage<PlayerFacingBlockMessage>(PlayerFacingBlockMessage(playerId, block));
    }
}
