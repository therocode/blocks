#include "playercontroller.hpp"
#include "../../world/worldinterface.hpp"
#include "../../rendering/renderingmessages.hpp"
#include "../../world/worldmessages.hpp"
#include "moveaction.hpp"

PlayerController::PlayerController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
    subscribe(mBus, *this);
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
    size_t playerId = received.playerId;
    glm::vec3 position = received.position;

    fea::EntityPtr playerEntity; 
    
    mBus.send(EntityRequestedMessage{"Player", [&] (fea::EntityPtr e) 
            {
                e->setAttribute("current_world", received.worldId);
                e->setAttribute("current_chunk", worldToChunk(position));
                e->setAttribute("position", position);
                playerEntity = e;
            }});

    mPlayerEntities.emplace(playerId, playerEntity);
    mBus.send(PlayerEntersChunkMessage{playerId, worldToChunk(position)});
    mBus.send(HighlightEntityAddRequestedMessage{received.worldId, playerEntity->getId(), worldToChunk(position)});

    ChunkCoord chunkAt = worldToChunk(position);

    mBus.send(PlayerConnectedToEntityMessage{(fea::EntityId)playerId, playerEntity->getId()});
}

void PlayerController::handleMessage(const PlayerDisconnectedMessage& received)
{
    size_t playerId = received.playerId;

    fea::EntityPtr entity = mPlayerEntities.at(playerId).lock();
    mBus.send(RemoveEntityRequestedMessage{entity->getId()});
    mBus.send(HighlightEntityRemoveRequestedMessage{entity->getAttribute<WorldId>("current_world"), entity->getId()});
    mPlayerEntities.erase(playerId);
}

void PlayerController::handleMessage(const PlayerActionMessage& received)
{
    size_t playerId = received.playerId;
    InputAction action = received.action;
    fea::EntityPtr entity = mPlayerEntities.at(playerId).lock();

    if(action == FORWARDS)
    {
            //fea::EntityPtr player = mPlayerEntities.at(playerId).lock();
            //player->setAttribute<MoveAction>("move_action", MoveAction::STANDING);
    }
    else if(action == JUMP)
    {
        mBus.send(EntityJumpMessage{entity->getId(), true});
    }
    else if(action == STOPJUMP)
	{
		mBus.send(EntityJumpMessage{entity->getId(), false});
	}
    else if(action == DIG)
    {
        // glm::vec3 worldPos = mPlayerEntities.at(playerId).lock()->getAttribute<VoxelWorldCoord>("block_facing");
		if(entity->getAttribute<bool>("is_facing_block"))
        {
			VoxelCoord voxel = entity->getAttribute<VoxelCoord>("block_facing");
			mBus.send(SetVoxelMessage{entity->getAttribute<WorldId>("current_world"), voxel, 0});
		}
    }
    else if(action == BUILD)
    {
        // glm::vec3 worldPos = mPlayerEntities.at(playerId).lock()->getAttribute<VoxelWorldCoord>("block_facing");
		if(entity->getAttribute<bool>("is_facing_block"))
        {
			VoxelCoord voxel = entity->getAttribute<VoxelCoord>("block_facing");
			uint32_t face = entity->getAttribute<uint32_t>("block_facing_face");
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

			mBus.send(SetVoxelMessage{entity->getAttribute<WorldId>("current_world"), voxel, (VoxelType)(rand() % 21 + 1)});//rand()%4 + 17));// (playerId + 1) % 20));
		}
    }
    else if(action == WARP)
    {
        WorldId oldWorld = entity->getAttribute<WorldId>("current_world");
        WorldId nextWorld = oldWorld == 0 ? 1 : 0;
        fea::EntityId entityId = entity->getId();

        mBus.send(HighlightEntityRemoveRequestedMessage{oldWorld, entityId});
        entity->setAttribute("current_world", nextWorld);
        mBus.send(HighlightEntityAddRequestedMessage{nextWorld, entityId, worldToChunk(entity->getAttribute<glm::vec3>("position"))});
    }
}

void PlayerController::handleMessage(const PlayerMoveDirectionMessage& received)
{
    size_t playerId = received.id;
    MoveDirection direction = received.direction;

    mPlayerEntities.at(playerId).lock()->setAttribute<MoveDirection>("move_direction", direction);
}

void PlayerController::handleMessage(const PlayerMoveActionMessage& received)
{
    size_t playerId = received.id;
    MoveAction moveAction = received.action;

    mPlayerEntities.at(playerId).lock()->setAttribute<MoveAction>("move_action", moveAction);
}

void PlayerController::handleMessage(const PlayerPitchYawMessage& received) //movement controller ni the future
{
	size_t playerId = received.playerId;

    float pitch = received.pitch;
    float yaw = received.yaw;

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
        mBus.send(RotateGfxEntityMessage{playerEntry->second.lock()->getId(), newPitch, newYaw});
        updateVoxelLookAt(playerId);
    }
}

void PlayerController::handleMessage(const EntityMovedMessage& received)
{
    size_t id = received.id;

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
    fea::EntityPtr entity = mPlayerEntities.at(playerId).lock();
    mBus.send(PlayerEntersChunkMessage{(fea::EntityId)playerId, chunk});
    mBus.send(HighlightEntityMoveRequestedMessage{entity->getAttribute<WorldId>("current_world"), entity->getId(), chunk});
    entity->setAttribute<ChunkCoord>("current_chunk", chunk);
}

void PlayerController::updateVoxelLookAt(size_t playerId)
{
    fea::EntityPtr entity = mPlayerEntities.at(playerId).lock();

    float pitch = entity->getAttribute<float>("pitch");
    float yaw = entity->getAttribute<float>("yaw");
    WorldId worldId = entity->getAttribute<WorldId>("current_world");
    glm::vec3 position = entity->getAttribute<glm::vec3>("position");

	glm::vec3 direction = glm::vec3(glm::cos(pitch) * glm::sin(yaw), glm::sin(pitch), glm::cos(pitch) * glm::cos(yaw));
	VoxelCoord block;
	uint32_t face;
	bool f = mWorldInterface.getVoxelAtRay(worldId, position + glm::vec3(0, 0.6f, 0), direction, 200.f, face, block);

    if(entity)
    {
        if(entity->getAttribute<uint32_t>("block_facing_face") != face){
            entity->setAttribute<uint32_t>("block_facing_face", face);
        }
        if(block != entity->getAttribute<VoxelCoord>("block_facing"))
        {
            entity->setAttribute<bool>("is_facing_block", f);
            entity->setAttribute<bool>("is_facing_block", f);
            entity->setAttribute<VoxelCoord>("block_facing", block);
            mBus.send(PlayerFacingBlockMessage{(fea::EntityId)playerId, block});
        }
    }
}
