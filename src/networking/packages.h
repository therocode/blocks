#pragma once
#include "package.h"
#include "script/scriptmessages.h"
#include "input/inputmessages.h"
#include "entity/entitymessages.h"
#include "world/worldmessages.h"
#include "rendering/renderingmessages.h"
#include "networkstd.h"

struct RebuildScriptsRequested_tag{};
using RebuildScriptsRequestedPackage = Package<RebuildScriptsRequested_tag, PackageType::REBUILD_SCRIPTS_REQUESTED, false, CHANNEL_DEFAULT, char>;
struct ChunkLoaded_tag{};
using ChunkLoadedPackage        = Package<ChunkLoaded_tag,         PackageType::CHUNK_LOADED,          false, CHANNEL_CHUNKS, ChunkCoord, RleIndexArray, RleSegmentArray>;
using ChunkDeletedPackage       = Package<ChunkLoaded_tag,         PackageType::CHUNK_DELETED,         false, CHANNEL_CHUNKS, ChunkCoord>;
struct VoxelSet_tag{};
using VoxelSetPackage           = Package<VoxelSet_tag,             PackageType::VOXEL_SET,             false, CHANNEL_CHUNKS, VoxelCoord, VoxelType>;
struct AddGfxEntity_tag{};
using GfxEntityAddedPackage     = Package<AddGfxEntity_tag,         PackageType::GFX_ENTITY_ADDED,      false, CHANNEL_ENTITY, size_t, glm::vec3>;
struct MoveGfxEntity_tag{};
using GfxEntityMovedPackage     = Package<MoveGfxEntity_tag,        PackageType::GFX_ENTITY_MOVED,      false, CHANNEL_ENTITY, size_t, glm::vec3>;
struct RotateGfxEntity_tag{};
using GfxEntityRotatedPackage   = Package<RotateGfxEntity_tag,      PackageType::GFX_ENTITY_ROTATED,    false, CHANNEL_ENTITY, size_t, float, float>;
struct RemoveGfxEntity_tag{};
using GfxEntityRemovedPackage   = Package<RemoveGfxEntity_tag,      PackageType::GFX_ENTITY_REMOVED,    false, CHANNEL_ENTITY, size_t>;
struct PlayerAction_tag{};
using PlayerActionPackage       = Package<PlayerAction_tag,         PackageType::PLAYER_ACTION,         false, CHANNEL_ENTITY, size_t, InputAction>;
struct PlayerMoveDirection_tag{};
using PlayerMoveDirectionPackage= Package<PlayerMoveDirection_tag,  PackageType::PLAYER_MOVE_DIRECTION, false, CHANNEL_ENTITY, size_t, int8_t, int8_t>;
struct PlayerMoveAction_tag{};
using PlayerMoveActionPackage   = Package<PlayerMoveAction_tag,     PackageType::PLAYER_MOVE_ACTION,    false, CHANNEL_ENTITY, size_t, MoveAction>;
using PlayerPitchYawPackage     = Package<PlayerAction_tag,         PackageType::PLAYER_PITCH_YAW,      true , CHANNEL_ENTITY, size_t, float, float>;
struct PlayerConnectedToEntity_tag{};
using PlayerConnectedToEntityPackage = Package<PlayerConnectedToEntity_tag, PackageType::PLAYER_CONNECTED_TO_ENTITY, false, CHANNEL_ENTITY, size_t, fea::EntityId>;
struct PlayerFacingBlock_tag{};
using PlayerFacingBlockPackage  = Package<PlayerFacingBlock_tag,    PackageType::PLAYER_FACING_BLOCK,   false, CHANNEL_ENTITY, size_t, float, float, float>;
struct PlayerId_tag{};
using PlayerIdPackage           = Package<PlayerId_tag,             PackageType::PLAYER_ID,             false, CHANNEL_ENTITY, size_t>;
