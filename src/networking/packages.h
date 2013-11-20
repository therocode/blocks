#pragma once
#include "package.h"
#include "script/scriptmessages.h"
#include "input/inputmessages.h"
#include "entity/entitymessages.h"
#include "world/worldmessages.h"
#include "rendering/renderingmessages.h"

enum PACKET_CHANNELS{
    CHANNEL_DEFAULT = 0,
    CHANNEL_CHUNKS,
    CHANNEL_ENTITY
};
using RebuildScriptsRequestedPackage = Package<RebuildScriptsRequested_tag, PackageType::REBUILD_SCRIPTS_REQUESTED, false, CHANNEL_DEFAULT, char>;

using ChunkLoadedPackage        = Package<ChunkCreated_tag,         PackageType::CHUNK_LOADED,          false, CHANNEL_CHUNKS, ChunkCoordinate, RleIndexArray, RleSegmentArray>;
using ChunkDeletedPackage       = Package<ChunkDeleted_tag,         PackageType::CHUNK_DELETED,         false, CHANNEL_CHUNKS, ChunkCoordinate>;
using VoxelSetPackage           = Package<VoxelSet_tag,             PackageType::VOXEL_SET,             false, CHANNEL_CHUNKS, ChunkCoordinate, VoxelCoordinate, VoxelType>;

using GfxEntityAddedPackage     = Package<AddGfxEntity_tag,         PackageType::GFX_ENTITY_ADDED,      false, CHANNEL_ENTITY, size_t, glm::vec3>;
using GfxEntityMovedPackage     = Package<MoveGfxEntity_tag,        PackageType::GFX_ENTITY_MOVED,      false, CHANNEL_ENTITY, size_t, glm::vec3>;
using GfxEntityRotatedPackage   = Package<RotateGfxEntity_tag,      PackageType::GFX_ENTITY_ROTATED,    false, CHANNEL_ENTITY, size_t, float, float>;
using GfxEntityRemovedPackage   = Package<RemoveGfxEntity_tag,      PackageType::GFX_ENTITY_REMOVED,    false, CHANNEL_ENTITY, size_t>;

using PlayerActionPackage       = Package<PlayerAction_tag,         PackageType::PLAYER_ACTION,         false, CHANNEL_ENTITY, size_t, InputAction>;
using PlayerMoveDirectionPackage= Package<PlayerMoveDirection_tag,  PackageType::PLAYER_MOVE_DIRECTION, false, CHANNEL_ENTITY, size_t, int8_t, int8_t>;
using PlayerMoveActionPackage   = Package<PlayerMoveAction_tag,     PackageType::PLAYER_MOVE_ACTION,    false, CHANNEL_ENTITY, size_t, MoveAction>;
using PlayerPitchYawPackage     = Package<PlayerAction_tag,         PackageType::PLAYER_PITCH_YAW,      true , CHANNEL_ENTITY, size_t, float, float>;
using PlayerConnectedToEntityPackage = Package<PlayerConnectedToEntity_tag, PackageType::PLAYER_CONNECTED_TO_ENTITY, false, CHANNEL_ENTITY, size_t, fea::EntityId>;
using PlayerFacingBlockPackage  = Package<PlayerFacingBlock_tag,    PackageType::PLAYER_FACING_BLOCK,   false, CHANNEL_ENTITY, size_t, float, float, float>;
using PlayerIdPackage           = Package<PlayerId_tag,             PackageType::PLAYER_ID,             false, CHANNEL_ENTITY, size_t>;
