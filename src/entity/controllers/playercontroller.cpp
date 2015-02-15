#include "playercontroller.hpp"
#include "../../gameinterface.hpp"
#include "../../rendering/renderingmessages.hpp"
#include "../../world/worldmessages.hpp"
#include "../../world/worldsystem.hpp"
#include "../../world/raycaster.hpp"
#include "moveaction.hpp"

PlayerController::PlayerController(fea::MessageBus& bus, GameInterface& gameInterface) :
    EntityController(bus),
    mGameInterface(gameInterface)
{
    subscribe(mBus, *this);
}

void PlayerController::handleMessage(const PlayerJoinedGameMessage& received)
{
    size_t playerId = received.playerId;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

    fea::EntityPtr playerEntity; 
    
    mBus.send(EntityRequestedMessage{"Player", [&] (fea::EntityPtr e) 
            {
                e->setAttribute("current_world", 0u);
                e->setAttribute("position", position);
                e->setAttribute("is_highlighting", true);
                e->setAttribute("highlight_radius", 6u);
                playerEntity = e;
            }});

    mEntities.emplace(playerId, playerEntity);
    mEntityIdToPlayerId.emplace(playerEntity->getId(), playerId);
    mBus.send(PlayerEntersChunkMessage{playerId, WorldToChunk::convert(position)});

    ChunkCoord chunkAt = WorldToChunk::convert(position);

    mBus.send(PlayerAttachedToEntityMessage{(fea::EntityId)playerId, playerEntity->getId(), playerEntity});

    mPitchYaws[playerId] = std::pair<float, float>(0.0f, 0.0f);
}

void PlayerController::handleMessage(const PlayerLeftGameMessage& received)
{
    size_t playerId = received.playerId;

    fea::EntityPtr entity = mEntities.at(playerId).lock();
    mEntityIdToPlayerId.erase(entity->getId());
    mBus.send(RemoveEntityRequestedMessage{entity->getId()});
    mEntities.erase(playerId);
    mPitchYaws.erase(playerId);
}

void PlayerController::handleMessage(const PlayerActionMessage& received)
{
    size_t playerId = received.playerId;
    InputAction action = received.action;
    fea::EntityPtr entity = mEntities.at(playerId).lock();

    if(action == FORWARDS)
    {
            //fea::EntityPtr player = mEntities.at(playerId).lock();
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
        // glm::vec3 worldPos = mEntities.at(playerId).lock()->getAttribute<VoxelWorldCoord>("block_facing");
		if(entity->getAttribute<bool>("is_facing_block"))
        {
			VoxelCoord voxel = entity->getAttribute<VoxelCoord>("block_facing");
			mBus.send(SetVoxelMessage{entity->getAttribute<WorldId>("current_world"), voxel, 0});
		}
    }
    else if(action == BUILD)
    {
        // glm::vec3 worldPos = mEntities.at(playerId).lock()->getAttribute<VoxelWorldCoord>("block_facing");
		if(entity->getAttribute<bool>("is_facing_block"))
        {
			VoxelCoord voxel = entity->getAttribute<VoxelCoord>("block_facing");
			uint32_t face = entity->getAttribute<uint32_t>("block_facing_face");
			ChunkCoord cc = VoxelToChunk::convert(voxel);
			ChunkVoxelCoord vc = VoxelToChunkVoxel::convert(voxel);
			// printf("ChunkCoord: %i, %i, %i. VoxelCoord: %i, %i, %i. World: %i, %i, %i\n", cc.x, cc.y, cc.z, vc.x, vc.y, vc.z, voxel.x, voxel.y, voxel.z);
			// printf("Face: %i\n", face);
			switch(face){
				case CUBE_TOP:
					voxel.y++;
					// printf("Top face\n");
					break;
				case CUBE_BOTTOM:
					voxel.y--;
					// printf("Bottom face\n");
					break;
				case CUBE_LEFT:
					voxel.x--;
					// printf("left face\n");
					break;
				case CUBE_RIGHT:
					voxel.x++;
					// printf("right face\n");
					break;
				case CUBE_FRONT:
					voxel.z--;
					// printf("front face\n");
					break;
				case CUBE_BACK:
					voxel.z++;
					// printf("back face\n");
					break;
			}

			mBus.send(SetVoxelMessage{entity->getAttribute<WorldId>("current_world"), voxel, (VoxelType)(playerId % 36 + 1)});//rand()%4 + 17));// (playerId + 1) % 20));
		}
    }
    else if(action == WARP)
    {
        WorldId oldWorld = entity->getAttribute<WorldId>("current_world");
        WorldId nextWorld = oldWorld == 0 ? 1 : 0;
        //fea::EntityId entityId = entity->getId();

        //entity->setAttribute("current_world", nextWorld);
        //mBus.send(PlayerEntersWorldMessage{playerId, nextWorld});
        //mBus.send(EntityEnteredWorldMessage{entityId, oldWorld, nextWorld});
        mBus.send(EntityWorldTransferRequestedMessage{entity, nextWorld});
    }
}

void PlayerController::handleMessage(const PlayerMoveDirectionMessage& received)
{
    size_t playerId = received.id;
    MoveDirection direction = received.direction;

    mEntities.at(playerId).lock()->setAttribute("move_direction", direction);
}

void PlayerController::handleMessage(const PlayerMoveActionMessage& received)
{
    size_t playerId = received.id;
    MoveAction moveAction = received.action;

    mEntities.at(playerId).lock()->setAttribute("move_action", moveAction);
}

void PlayerController::handleMessage(const PlayerPitchYawMessage& received) //movement controller ni the future
{
	size_t playerId = received.playerId;

    FEA_ASSERT(mPitchYaws.count(playerId) != 0, "No player exists that is being rotated!");
    auto& pitchYaw = mPitchYaws.at(playerId);
    pitchYaw.first += received.pitch;
    pitchYaw.second += received.yaw;

    auto playerEntry = mEntities.find(playerId);
    if(playerEntry != mEntities.end())
    {
        fea::EntityPtr entity = playerEntry->second.lock();

		if(pitchYaw.first >= glm::pi<float>() * 0.5f)
			pitchYaw.first = glm::pi<float>() * 0.5f - 0.001f;
		if(pitchYaw.first <= -glm::pi<float>() * 0.5f)
			pitchYaw.first = -glm::pi<float>() * 0.5f + 0.001f;

        glm::quat newOrientation = glm::quat(glm::vec3(pitchYaw.first, pitchYaw.second, 0.0f));

        entity->setAttribute("orientation", newOrientation);

        mBus.send(EntityOrientedMessage{playerEntry->second.lock()->getId(), newOrientation});
        updateVoxelLookAt(playerId);
    }
}

void PlayerController::handleMessage(const EntityMovedMessage& received)
{
    size_t id = received.entityId;

    if(mEntityIdToPlayerId.find(id) != mEntityIdToPlayerId.end())
    {
        size_t playerId = mEntityIdToPlayerId.at(id);
        updateVoxelLookAt(playerId);

        fea::EntityPtr entity = mEntities.at(playerId).lock();
        //updating current chunk
        ChunkCoord newChunk = WorldToChunk::convert(received.newPosition);

        if(WorldToChunk::convert(received.oldPosition) != newChunk)
        {
            playerEntersChunk(playerId, newChunk);
        }

        mBus.send(PlayerEntityMovedMessage{playerId, received.newPosition});
    }

}

void PlayerController::handleMessage(const EntityTransferredWorldMessage& received)
{
    size_t id = received.entityId;

    if(mEntityIdToPlayerId.find(id) != mEntityIdToPlayerId.end())
    {
        size_t playerId = mEntityIdToPlayerId.at(id);
        mBus.send(PlayerEntersWorldMessage{playerId, received.newWorld});
    }
}

void PlayerController::playerEntersChunk(size_t playerId, const ChunkCoord& chunk)
{
    fea::EntityPtr entity = mEntities.at(playerId).lock();
    mBus.send(PlayerEntersChunkMessage{(fea::EntityId)playerId, chunk});
}

void PlayerController::updateVoxelLookAt(size_t playerId)
{
    fea::EntityPtr entity = mEntities.at(playerId).lock();

    glm::quat orientation = entity->getAttribute<glm::quat>("orientation");

    WorldId worldId = entity->getAttribute<WorldId>("current_world");
    glm::vec3 position = entity->getAttribute<glm::vec3>("position");

	glm::vec3 direction = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
	VoxelCoord block;
	uint32_t face;
	bool f = RayCaster::getVoxelAtRay(mGameInterface.getWorldSystem().getWorldVoxels(worldId), position + glm::vec3(0, 0.6f, 0), direction, 200.f, face, block);

    if(entity)
    {
        if(entity->getAttribute<uint32_t>("block_facing_face") != face){
            entity->setAttribute("block_facing_face", face);
        }
        if(block != entity->getAttribute<VoxelCoord>("block_facing"))
        {
            entity->setAttribute("is_facing_block", f);
            entity->setAttribute("is_facing_block", f);
            entity->setAttribute("block_facing", block);
            mBus.send(PlayerFacingBlockMessage{(fea::EntityId)playerId, block});
        }
    }
}
