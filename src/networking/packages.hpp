#pragma once
#include "package.hpp"
#include "networkstd.hpp"
#include "../input/inputactions.hpp"
#include "../world/chunk.hpp"
#include "../entity/controllers/moveaction.hpp"
#include <fea/entitysystem.hpp>

struct RebuildScriptsRequested_tag{};
using RebuildScriptsRequestedPackage = Package<RebuildScriptsRequested_tag, PackageType::REBUILD_SCRIPTS_REQUESTED, char>;

struct ChunkLoaded_tag{};
using ChunkLoadedPackage        = Package<ChunkLoaded_tag,         PackageType::CHUNK_LOADED, ChunkCoord, RleIndexArray, RleSegmentArray>;

struct ChunkDeleted_tag{};
using ChunkDeletedPackage       = Package<ChunkDeleted_tag,         PackageType::CHUNK_DELETED, ChunkCoord>;

struct VoxelSet_tag{};
using VoxelSetPackage           = Package<VoxelSet_tag,             PackageType::VOXEL_SET, VoxelCoord, VoxelType>;

struct AddGfxEntity_tag{};
using GfxEntityAddedPackage     = Package<AddGfxEntity_tag,         PackageType::GFX_ENTITY_ADDED, size_t, glm::vec3>;

struct MoveGfxEntity_tag{};
using GfxEntityMovedPackage     = Package<MoveGfxEntity_tag,        PackageType::GFX_ENTITY_MOVED, size_t, glm::vec3>;

struct RotateGfxEntity_tag{};
using GfxEntityRotatedPackage   = Package<RotateGfxEntity_tag,      PackageType::GFX_ENTITY_ROTATED, size_t, float, float>;

struct RemoveGfxEntity_tag{};
using GfxEntityRemovedPackage   = Package<RemoveGfxEntity_tag,      PackageType::GFX_ENTITY_REMOVED, size_t>;

struct PlayerAction_tag{};
using PlayerActionPackage       = Package<PlayerAction_tag,         PackageType::PLAYER_ACTION, size_t, InputAction>;

struct PlayerMoveDirection_tag{};
using PlayerMoveDirectionPackage= Package<PlayerMoveDirection_tag,  PackageType::PLAYER_MOVE_DIRECTION, size_t, int8_t, int8_t>;

struct PlayerMoveAction_tag{};
using PlayerMoveActionPackage   = Package<PlayerMoveAction_tag,     PackageType::PLAYER_MOVE_ACTION, size_t, MoveAction>;

struct PlayerPitchYaw_tag{};
using PlayerPitchYawPackage     = Package<PlayerPitchYaw_tag,         PackageType::PLAYER_PITCH_YAW, size_t, float, float>;

struct PlayerConnectedToEntity_tag{};
using PlayerConnectedToEntityPackage = Package<PlayerConnectedToEntity_tag, PackageType::PLAYER_CONNECTED_TO_ENTITY, size_t, fea::EntityId>;

struct PlayerFacingBlock_tag{};
using PlayerFacingBlockPackage  = Package<PlayerFacingBlock_tag,    PackageType::PLAYER_FACING_BLOCK, size_t, float, float, float>;

struct PlayerId_tag{};
using PlayerIdPackage           = Package<PlayerId_tag,             PackageType::PLAYER_ID, size_t>;
