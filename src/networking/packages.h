#pragma once
#include "package.h"
#include "../script/scriptmessages.h"
#include "../input/inputmessages.h"
#include "../entity/entitymessages.h"

using RebuildScriptsRequestedPackage = Package<RebuildScriptsRequested_tag, char>;

using ChunkLoadedPackage = Package<ChunkCreated_tag, ChunkCoordinate, RleIndexArray, RleSegmentArray>;
using ChunkDeletedPackage = Package<ChunkDeleted_tag, ChunkCoordinate>;

using GfxEntityAddedPackage = Package<AddGfxEntity_tag, size_t, glm::vec3>;
using GfxEntityMovedPackage = Package<MoveGfxEntity_tag, size_t, glm::vec3>;
using GfxEntityRotatedPackage = Package<RotateGfxEntity_tag, size_t, float, float>;
using GfxEntityRemovedPackage = Package<RemoveGfxEntity_tag, size_t>;

using PlayerActionPackage = Package<PlayerAction_tag, size_t, InputAction>;
using PlayerMoveDirectionPackage = Package<PlayerMoveDirection_tag, size_t, int8_t, int8_t>;
using PlayerMoveActionPackage = Package<PlayerMoveAction_tag, size_t, MoveAction>;
using PlayerPitchYawPackage = Package<PlayerAction_tag, size_t, float, float>;
using PlayerConnectedToEntityPackage = Package<PlayerConnectedToEntity_tag, size_t, fea::EntityId>;
using PlayerFacingBlockPackage = Package<PlayerFacingBlock_tag, size_t, float, float, float>;
using PlayerIdPackage = Package<PlayerId_tag, size_t>;
