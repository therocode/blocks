#pragma once
#include "package.h"
#include "script/scriptmessages.h"
#include "input/inputmessages.h"
#include "entity/entitymessages.h"
#include "world/worldmessages.h"
#include "rendering/renderingmessages.h"

using RebuildScriptsRequestedPackage = Package<RebuildScriptsRequested_tag, PackageType::REBUILD_SCRIPTS_REQUESTED, false, char>;

using ChunkLoadedPackage = Package<ChunkCreated_tag, PackageType::CHUNK_LOADED, false, ChunkCoordinate, RleIndexArray, RleSegmentArray>;
using ChunkDeletedPackage = Package<ChunkDeleted_tag, PackageType::CHUNK_DELETED, false, ChunkCoordinate>;
using VoxelSetPackage = Package<VoxelSet_tag, PackageType::VOXEL_SET, false, ChunkCoordinate, VoxelCoordinate, VoxelType>;

using GfxEntityAddedPackage = Package<AddGfxEntity_tag, PackageType::GFX_ENTITY_ADDED, false, size_t, glm::vec3>;
using GfxEntityMovedPackage = Package<MoveGfxEntity_tag, PackageType::GFX_ENTITY_MOVED, true, size_t, glm::vec3>;
using GfxEntityRotatedPackage = Package<RotateGfxEntity_tag, PackageType::GFX_ENTITY_ROTATED, true, size_t, float, float>;
using GfxEntityRemovedPackage = Package<RemoveGfxEntity_tag, PackageType::GFX_ENTITY_REMOVED, false, size_t>;

using PlayerActionPackage = Package<PlayerAction_tag, PackageType::PLAYER_ACTION, false, size_t, InputAction>;
using PlayerMoveDirectionPackage = Package<PlayerMoveDirection_tag, PackageType::PLAYER_MOVE_DIRECTION, false, size_t, int8_t, int8_t>;
using PlayerMoveActionPackage = Package<PlayerMoveAction_tag, PackageType::PLAYER_MOVE_ACTION, false, size_t, MoveAction>;
using PlayerPitchYawPackage = Package<PlayerAction_tag, PackageType::PLAYER_PITCH_YAW, true, size_t, float, float>;
using PlayerConnectedToEntityPackage = Package<PlayerConnectedToEntity_tag, PackageType::PLAYER_CONNECTED_TO_ENTITY, false, size_t, fea::EntityId>;
using PlayerFacingBlockPackage = Package<PlayerFacingBlock_tag, PackageType::PLAYER_FACING_BLOCK, false, size_t, float, float, float>;
using PlayerIdPackage = Package<PlayerId_tag, PackageType::PLAYER_ID, false, size_t>;
