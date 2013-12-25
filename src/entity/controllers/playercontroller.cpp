#include "playercontroller.h"
#include "../../world/worldinterface.h"
#include "../../rendering/renderingmessages.h"
#include "world/worldmessages.h"
#include "moveaction.h"
#include <iostream>

PlayerController::PlayerController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
    mBus.addMessageSubscriber<PlayerJoinedMessage>(*this);
    mBus.addMessageSubscriber<PlayerDisconnectedMessage>(*this);
    mBus.addMessageSubscriber<PlayerActionMessage>(*this);
    mBus.addMessageSubscriber<PlayerMoveDirectionMessage>(*this);
    mBus.addMessageSubscriber<PlayerMoveActionMessage>(*this);
    mBus.addMessageSubscriber<PlayerPitchYawMessage>(*this);
    mBus.addMessageSubscriber<EntityMovedMessage>(*this);
}

PlayerController::~PlayerController()
{
    mBus.removeMessageSubscriber<PlayerJoinedMessage>(*this);
    mBus.removeMessageSubscriber<PlayerDisconnectedMessage>(*this);
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

void PlayerController::onFrame(int dt)
{
}

void PlayerController::handleMessage(const PlayerJoinedMessage& received)
{
    size_t playerId;
    glm::vec3 position;

    std::tie(playerId, position) = received.data;

    position = glm::vec3(-512, 0, -512);

    fea::WeakEntityPtr playerEntity = mWorldInterface.createEntity("Player", position);
    std::cout << "created player entity and it's id is " << playerEntity.lock()->getId() << "\n";
    mPlayerEntities.emplace(playerId, playerEntity);
    playerEntity.lock()->setAttribute<ChunkCoord>("current_chunk", worldToChunk(position));
    mBus.sendMessage(PlayerEntersChunkMessage(playerId, worldToChunk(position)));

    ChunkCoord chunkAt = worldToChunk(position);

    for(int32_t x = -10; x < 10; x++)
    for(int32_t y = -10; y < 10; y++)
    for(int32_t z = -10; z < 10; z++)
    {
        mBus.sendMessage(ChunkRequestedMessage(chunkAt + ChunkCoord(x, y, z)));
    }

    mBus.sendMessage(PlayerConnectedToEntityMessage(playerId, playerEntity.lock()->getId()));
}

void PlayerController::handleMessage(const PlayerDisconnectedMessage& received)
{
    size_t playerId;

    std::tie(playerId) = received.data;

    mBus.sendMessage<RemoveEntityMessage>(RemoveEntityMessage(mPlayerEntities.at(playerId).lock()->getId()));
    mPlayerEntities.erase(playerId);
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
    }
    else if(action == STOPJUMP)
	{
		mBus.sendMessage<EntityJumpMessage>(EntityJumpMessage(mPlayerEntities.at(playerId).lock()->getId(), false));
	}
    else if(action == DIG)
    {
        // glm::vec3 worldPos = mPlayerEntities.at(playerId).lock()->getAttribute<VoxelWorldCoord>("block_facing");
		if(mPlayerEntities.at(playerId).lock()->getAttribute<bool>("is_facing_block")){
			VoxelCoord voxel = mPlayerEntities.at(playerId).lock()->getAttribute<VoxelCoord>("block_facing");
			mBus.sendMessage<SetVoxelMessage>(SetVoxelMessage(voxel, 0));
		}
    }
    else if(action == BUILD)
    {
        // glm::vec3 worldPos = mPlayerEntities.at(playerId).lock()->getAttribute<VoxelWorldCoord>("block_facing");
		if(mPlayerEntities.at(playerId).lock()->getAttribute<bool>("is_facing_block")){
			VoxelCoord voxel = mPlayerEntities.at(playerId).lock()->getAttribute<VoxelCoord>("block_facing");
			int face = mPlayerEntities.at(playerId).lock()->getAttribute<int>("block_facing_face");
			ChunkCoord cc = voxelToChunk(voxel);
			ChunkVoxelCoord vc = voxelToChunkVoxel(voxel);
			// printf("ChunkCoord: %i, %i, %i. VoxelCoord: %i, %i, %i. World: %i, %i, %i\n", cc.x, cc.y, cc.z, vc.x, vc.y, vc.z, voxel.x, voxel.y, voxel.z);
			// printf("Face: %i\n", face);
			switch(face){
				case FACE_TOP:
					voxel.y++;
					// printf("Top face\n");
					break;
				case FACE_BOTTOM:
					voxel.y--;
					// printf("Bottom face\n");
					break;
				case FACE_LEFT:
					voxel.x--;
					// printf("left face\n");
					break;
				case FACE_RIGHT:
					voxel.x++;
					// printf("right face\n");
					break;
				case FACE_FRONT:
					voxel.z--;
					// printf("front face\n");
					break;
				case FACE_BACK:
					voxel.z++;
					// printf("back face\n");
					break;
			}

			mBus.sendMessage<SetVoxelMessage>(SetVoxelMessage(voxel,21));//rand()%4 + 17));// (playerId + 1) % 20));
		}
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

        if(worldToChunk(position) != entity->getAttribute<ChunkCoord>("current_chunk"))
        {
            playerEntersChunk(id, worldToChunk(position));
        }
    }

}

void PlayerController::playerEntersChunk(size_t playerId, const ChunkCoord& chunk)
{
    mBus.sendMessage<PlayerEntersChunkMessage>(PlayerEntersChunkMessage(playerId, chunk));
    mPlayerEntities.at(playerId).lock()->setAttribute<ChunkCoord>("current_chunk", chunk);
}

void PlayerController::updateVoxelLookAt(size_t playerId)
{
    fea::EntityPtr entity = mPlayerEntities.at(playerId).lock();


    float pitch = entity->getAttribute<float>("pitch");
    float yaw = entity->getAttribute<float>("yaw");
    glm::vec3 position = entity->getAttribute<glm::vec3>("position");

	glm::vec3 direction = glm::vec3(glm::cos(pitch) * glm::sin(yaw), glm::sin(pitch), glm::cos(pitch) * glm::cos(yaw));
	VoxelCoord block;
	int face;
	bool f = mWorldInterface.getVoxelAtRay(position + glm::vec3(0, 0.6f, 0), direction, 200.f, face, block);

	if(entity->getAttribute<int>("block_facing_face") != face){
		entity->setAttribute<int>("block_facing_face", face);
	}
    if(block != entity->getAttribute<VoxelCoord>("block_facing"))
	{
		entity->setAttribute<bool>("is_facing_block", f);
		entity->setAttribute<bool>("is_facing_block", f);
        entity->setAttribute<VoxelCoord>("block_facing", block);
        mBus.sendMessage<PlayerFacingBlockMessage>(PlayerFacingBlockMessage(playerId, block));
    }
}
